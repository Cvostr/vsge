using System;
using System.Collections.Generic;

public abstract class View
{
    public View()
    {
        _children = new List<View>();
        _parent = null;

        _rotation = 0;
        _visible = true;
    }

    private List<View> _children;
    private View _parent;

    // For rendering
    protected Rect _globalBounds;

    private bool _visible;
    protected float _rotation;

    // Normalized
    private Vec2 _anchorMin;
    private Vec2 _anchorMax;
    private Vec2 _pivot;

    // Pixels
    private Vec2 _position;
    private Vec2 _size;


    public void AddChild(View view)
    {
        _children.Add(view);
        view._parent = this;
    }

    public void RemoveChild(View view)
    {
        view._parent = null;
        _children.Remove(view);
    }

    public View GetParent()
    {
        return _parent;
    }

    public bool HasParent()
    {
        return _parent != null;
    }

    public void SetVisible(bool visible)
    {
        _visible = visible;
    }

    public bool IsVisible()
    {
        return _visible;
    }

    public float GetRotation()
    {
        return _rotation;
    }

    public void SetRotation(float rotation)
    {
        _rotation = rotation;
    }

    public void SetAnchors(in Vec2 min, in Vec2 max)
    {
        _anchorMin = min;
        _anchorMax = max;
    }

    public void SetPivot(in Vec2 pivot)
    {
        _pivot = pivot;
    }

    public void SetPosition(in Vec2 position)
    {
        _position = position;
    }

    public void SetSize(in Vec2 size)
    {
        _size = size;
    }

    public void Process()
    {
        if (_visible == false)
            return;

        // Graphics
        Render();

        // Process children
        foreach (View child in _children)
        {
            child.Process();
        }
    }

    public void UpdateGlobalRect(in Rect parent)
    {
        Vec2 anchorMinPos = parent.Pos + parent.Size * _anchorMin;
        Vec2 anchorMaxPos = parent.Pos + parent.Size * _anchorMax;
        Vec2 anchorBoxSize = anchorMaxPos - anchorMinPos;

        // Absolute size of element
        _globalBounds.Size = anchorBoxSize + _size;

        // Pivot
        Vec2 anchorPivotPos = anchorMinPos + anchorBoxSize * _pivot;
        Vec2 widgetPivotPos = anchorPivotPos + _position;

        // Pos - Upper Left corner
        _globalBounds.Pos = widgetPivotPos - _globalBounds.Size * _pivot;

        // Update children
        foreach (View child in _children)
        {
            if (child.IsVisible())
            {
                child.UpdateGlobalRect(_globalBounds);
            }
        }
    }

    public abstract void Render();
}