#pragma once 
#include "Math/Vector3.h"
#include "Math/OtherMath.h"

#include <vector>




namespace Matrix
{
	using namespace Math;
	struct PointLight
	{

		Math::Vector3 m_position;
		// radiant flux in W
		Math::Vector3 m_flux;

		// calculate an appropriate radius for light culling
		// a windowing function in the shader will perform a smooth transition to zero
		// this is not physically based and usually artist controlled
		float calculateRadius() const
		{
			// radius = where attenuation would lead to an intensity of 1W/m^2
			const float INTENSITY_CUTOFF = 1.0f;
			const float ATTENTUATION_CUTOFF = 0.05f;
			Vector3     intensity = m_flux / (4.0f * Math::PI);
			float       maxIntensity = Math::getMaxElement(intensity.x, intensity.y, intensity.z);
			float       attenuation = Math::Max(INTENSITY_CUTOFF, ATTENTUATION_CUTOFF * maxIntensity) / maxIntensity;
			return 1.0f / sqrtf(attenuation);
		}
	};

	struct AmbientLight
	{
		Vector3 m_irradiance;
	};

	struct PDirectionalLight
	{
		Vector3 m_direction;
		Vector3 m_color;
	};
}