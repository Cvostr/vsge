#pragma once

#include <Core/VarTypes/Base.hpp>
#include "Texture.hpp"
#include "GpuObject.hpp"
#include <vector>

namespace VSGE {
	class Framebuffer : public IGpuObject {
	protected:
		uint32 _width;
		uint32 _height;

		std::vector<Texture*> _attachments;
		Texture* _depthAttachment;

	public:
		/// <summary>
		/// Add existing texture as attachment to framebuffer
		/// </summary>
		/// <param name="attachment">- pointer to texture</param>
		virtual void AddAttachment(Texture* attachment) = 0;
		/// <summary>
		/// Create new texture and add it as attachment to framebuffer
		/// with size of framebuffer
		/// </summary>
		/// <param name="Format">- format of new texture</param>
		/// <param name="layers">- count of layers of new texture</param>
		virtual void AddAttachment(TextureFormat format = TextureFormat::FORMAT_RGBA, uint32 layers = 1, bool cubemap = false) = 0;
		/// <summary>
		/// Create and set new depth attachment
		/// </summary>
		/// <param name="Format">- format of new depth texture</param>
		/// <param name="Layers">- count of layers of new depth texture</param>
		virtual void AddDepth(TextureFormat format = FORMAT_DEPTH_24_STENCIL_8, uint32 layers = 1, bool cubemap = false) = 0;
		/// <summary>
		/// Change size property of framebuffer
		/// </summary>
		/// <param name="width">- new width of fb</param>
		/// <param name="height">- new height of fb</param>
		void SetSize(uint32 width, uint32 height){
			_width = width;
			_height = height;
		}
		/// <summary>
		/// Change size of framebuffer and resize all attachments in this framebuffer
		/// </summary>
		/// <param name="width">- new width of fb</param>
		/// <param name="height">- new height of fb</param>
		virtual void Resize(uint32 width, uint32 height) = 0;

		Texture** GetColorAttachments() {
			return _attachments.data();
		}

		Texture* GetDepthAttachment() {
			return _depthAttachment;
		}
		/// <summary>
		/// Create framebuffer gpu object from specified attachments
		/// </summary>
		//virtual bool Create() = 0;
		/// <summary>
		/// Destroy created framebuffer
		/// </summary>
		virtual void Destroy() = 0;

		Framebuffer() :
			_width(0),
			_height(0),
			_depthAttachment(nullptr)
		{}

		Framebuffer(uint32 width, uint32 height) :
			_width(width),
			_height(height),
			_depthAttachment(nullptr)
		{}
	};
}