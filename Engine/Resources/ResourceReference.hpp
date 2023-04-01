#pragma once

#include <string>
#include "Resource.hpp"
#include "ResourceCache.hpp"

namespace VSGE{
    class ResourceReference {
		ResourceType _resourceType;
		Resource* _resourcePointer;
		Guid _resourceId;			//ID �������, ���� ��������� �� �������� ������ - ��� �� ��������
		std::string _childName;		//���� �� ������ - ������ ��������� �� �������� ������

		Resource* ResolvePointer() const; //��������� ��������� �� ����
		void UpdatePointer();
	public:
		
		ResourceType GetResourceType();

		void SetResourceType(ResourceType type);

		void SetPointerToNull();

		template<class T>
		T* GetResource() {
			if (!_resourcePointer)
				UpdatePointer();

			return (T*)_resourcePointer;
		}

		Resource* GetResource();

		Resource* GetParentResource();

		const std::string GetResourceName() const;

		const Guid& GetId() const;

		void SetResource(const Guid& id);

		void SetResource(const Guid& parentId, const std::string& childName);

		void SetResource(Resource* resource);

		bool IsResourceSpecified();
        
		ResourceReference();
		~ResourceReference();
	};
}