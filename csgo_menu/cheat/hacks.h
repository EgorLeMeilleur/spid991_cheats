#pragma once
#include "memory.h"
#include <thread>
#include <array>
#include <numbers>
#include <cmath>

namespace hacks
{
	void VisualsThread(const Memory& memory) noexcept;
	void PlayerThread(const Memory& memory) noexcept;

	constexpr const int GetWeapon(const short& item_definition)
	{
		switch (item_definition)
		{
		case 1: return 37; //deagle
		case 4: return 353; //glock
		case 7: return 542; //ak
		case 9: return 255; //awp
		case 60: return 548; //m4a1
		case 61: return 504; //usp
		}
	}

	struct Color3
	{
		std::uint8_t r{ }, g{ }, b{ };
	};

	struct Vector3
	{
		constexpr Vector3(const float x_ = 0.f, const float y_  = 0.f, const float z_ = 0.f) noexcept : x(x_), y(y_), z(z_) {}
		float x, y, z;

		constexpr const Vector3& operator+(const Vector3& other) const noexcept
		{
			return Vector3{ x + other.x, y + other.y, z + other.z };
		}

		constexpr const Vector3& operator-(const Vector3& other) const noexcept
		{
			return Vector3{ x - other.x, y - other.y, z - other.z };
		}

		constexpr const Vector3& operator*(float factor) const noexcept
		{
			return Vector3{ x * factor, y * factor, z * factor };
		}

		constexpr const Vector3& operator/(float factor) const noexcept
		{
			return Vector3{ x / factor, y / factor, z / factor };
		}

		constexpr const Vector3& ToAngle() const noexcept
		{
			return Vector3{ std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
							std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
							0.0f };
		}

		constexpr const bool IsZero() const noexcept
		{
			return x == 0.f && y == 0.f && z == 0.f;
		}
	};

	constexpr Vector3 CalculateAngle(const Vector3& localPosition, const Vector3& enemyPosition, const Vector3& viewAngles) noexcept
	{
		return ((enemyPosition - localPosition).ToAngle() - viewAngles);
	}
}

