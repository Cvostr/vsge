#pragma once

#include <vector>

namespace VSGE {
	class View {
	private:
		std::vector<View*> _children;

		bool IsChildExist(View* view);
	public:

		View();
		~View();

		void AddChild(View* view);
		void RemoveChild(View* view);
		std::vector<View*>& GetChildren();
	};
}