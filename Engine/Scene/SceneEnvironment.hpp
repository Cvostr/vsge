#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Resources/ResourceReference.hpp>
#include <Math/Color.hpp>
#include <Math/Vec3.hpp>
#include <Math/Mat4.hpp>
#include <vector>

namespace VSGE{
    class SceneEnvironmentSettings {
	private:
		float _shadow_distance;
		uint32 _shadow_cascades_count;
		std::vector<float> _cascade_dists;
		std::vector<float> _cascade_depths;
		Color _ambient_color;
	public:
		
		ResourceReference _skybox_material;

		void UpdateShadows();

		const Color& GetAmbientColor();

		void SetAmbientColor(const Color& color);

		float GetMaxShadowDistance();

		void SetMaxShadowDistance(float distance);

		uint32 GetShadowCascadesCount();

		float* GetCascadeDists();

		float* GetCascadeDepths();

		int GetCascadeByDistance(Vec3 pos, const Mat4& cam_view_proj);

		void SetShadowCascadesCount(uint32 cascades);
        
		SceneEnvironmentSettings();
	};
}