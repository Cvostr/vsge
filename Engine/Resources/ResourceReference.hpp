#pragma once

#include <string>
#include "Resource.hpp"

namespace VSGE{
    class ResourceReference {
		ResourceType _resourceType;
		Resource* _resourcePointer;
		std::string _resourceName;
		std::string _parentName;
	public:
		
		ResourceType GetResourceType();

		void SetResourceType(ResourceType type);

		template<class T>
		T* GetResource() {
			return static_cast<T*>(GetResource());
		}

		Resource* GetResource();

		Resource* GetParentResource();

		const std::string& GetResourceName() const;

		void SetResource(const std::string& resourceName);

        const std::string& GetResourceParentName() const;

		void SetParentResource(const std::string& parentName);

		void SetResource(Resource* resource);

		bool IsResourceSpecified();

		void operator=(ResourceReference& ref);
        
		ResourceReference();
	};
}