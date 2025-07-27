#pragma once
#include "Particle.h"
#include "Random.h"
#include "Vector3.h"
#include <list>

// エミッター
struct Emitter
{
public:
	// 形状
	enum class Shape
	{
		Point,
		Box,
		Sphere
	};

	bool mIsPlaying = true;
	Vector3 mPosition;
	float mLife = 10.0f;
	float mCurrTime;
	uint32_t mAmount = 3;
	float mFrequency = 1.0f;
	float mFrequencyTime;
	bool mIsLoop = true;
	float mDelay = 3.0f;
	// 形状
	Shape mShape = Shape::Point;
	Vector3 mBoxMin;
	Vector3 mBoxMax;
	float mSphereRadius;

	// パーティクル初期化
	template <class T>
	struct InitData
	{
		T mMin;
		T mMax;
		T Rand()
		{
			return Random::Rand(mMin, mMax);
		}
	};
	InitData<float> mInitLife = { 1.0f,2.0f };
	InitData<Vector3> mInitVel = { -Vector3::kOne,Vector3::kOne };
	InitData<Vector3> mInitAccel = { -Vector3::kOne / 10.0f,Vector3::kOne / 10.0f };
	InitData<float> mInitSScale = { 5.0f,10.0f };
	InitData<float> mInitEScale = { 0.0f,0.0f };
	InitData<Vector3> mInitSRot = { Vector3::kZero,Vector3::kZero };//{ -Vector3::kOne * MyMath::k2Pi,Vector3::kOne * MyMath::k2Pi };
	InitData<Vector3> mInitERot = { Vector3::kZero,Vector3::kZero };//{ -Vector3::kOne * MyMath::k2Pi,Vector3::kOne * MyMath::k2Pi };
	InitData<Color> mInitSColor = { Color::kBlack,Color::kWhite };
	InitData<Color> mInitEColor = { Color::kBlack,Color::kWhite };

	void Update(float deltaTime)
	{
		if (mIsPlaying)
		{
			mCurrTime += deltaTime;
			if (mCurrTime >= mLife)
			{
				mFrequencyTime = 0.0f;
				if (mIsLoop)
				{
					mCurrTime = 0.0f;
				}
			}
			if (mCurrTime >= mDelay &&
				mCurrTime < mLife)
			{
				mFrequencyTime += deltaTime;
			}
		}
	}

	std::list<Particle> Emit()
	{
		Clamp();
		std::list<Particle> particles;
		for (uint32_t i = 0; i < mAmount; ++i)
		{
			Particle p;
			p.mCurrTime = 0.0f;
			p.mLife = mInitLife.Rand();
			switch (mShape)
			{
			case Shape::Point:
				p.mPosition = mPosition;
				break;
			case Shape::Box:
				p.mPosition = mPosition + Random::Rand(mBoxMin, mBoxMax);
				break;
			case Shape::Sphere:
				float radius = Random::Rand(0.0f, mSphereRadius);
				float theta = Random::Rand(0.0f, RdMath::kPi);
				float phi = Random::Rand(0.0f, RdMath::k2Pi);
				p.mPosition = mPosition + Vector3(
					radius * sinf(theta) * cosf(phi),
					radius * sinf(theta) * sinf(phi),
					radius * cosf(theta));
				break;
			}
			p.mVel = mInitVel.Rand();
			p.mAccel = mInitAccel.Rand();
			p.mSScale = mInitSScale.Rand();
			p.mEScale = mInitEScale.Rand();
			p.mSRot = mInitSRot.Rand();
			p.mERot = mInitERot.Rand();
			p.mSColor = mInitSColor.Rand();
			p.mEColor = mInitEColor.Rand();
			particles.push_back(p);
		}
		return particles;
	}

	void Clamp()
	{
		mInitLife.mMin = RdMath::Min(mInitLife.mMin, mInitLife.mMax);
		mInitVel.mMin = Vector3(
			RdMath::Min(mInitVel.mMin.x, mInitVel.mMax.x),
			RdMath::Min(mInitVel.mMin.y, mInitVel.mMax.y),
			RdMath::Min(mInitVel.mMin.z, mInitVel.mMax.z));
		mInitAccel.mMin = Vector3(
			RdMath::Min(mInitAccel.mMin.x, mInitAccel.mMax.x),
			RdMath::Min(mInitAccel.mMin.y, mInitAccel.mMax.y),
			RdMath::Min(mInitAccel.mMin.z, mInitAccel.mMax.z));
		mInitSScale.mMin = RdMath::Min(mInitSScale.mMin, mInitSScale.mMax);
		mInitEScale.mMin = RdMath::Min(mInitEScale.mMin, mInitEScale.mMax);
		mInitSRot.mMin = Vector3(
			RdMath::Min(mInitSRot.mMin.x, mInitSRot.mMax.x),
			RdMath::Min(mInitSRot.mMin.y, mInitSRot.mMax.y),
			RdMath::Min(mInitSRot.mMin.z, mInitSRot.mMax.z));
		mInitERot.mMin = Vector3(
			RdMath::Min(mInitERot.mMin.x, mInitERot.mMax.x),
			RdMath::Min(mInitERot.mMin.y, mInitERot.mMax.y),
			RdMath::Min(mInitERot.mMin.z, mInitERot.mMax.z));
		mInitSColor.mMin = Color(
			RdMath::Min(mInitSColor.mMin.r, mInitSColor.mMax.r),
			RdMath::Min(mInitSColor.mMin.g, mInitSColor.mMax.g),
			RdMath::Min(mInitSColor.mMin.b, mInitSColor.mMax.b),
			RdMath::Min(mInitSColor.mMin.a, mInitSColor.mMax.a));
		mInitEColor.mMin = Color(
			RdMath::Min(mInitEColor.mMin.r, mInitEColor.mMax.r),
			RdMath::Min(mInitEColor.mMin.g, mInitEColor.mMax.g),
			RdMath::Min(mInitEColor.mMin.b, mInitEColor.mMax.b),
			RdMath::Min(mInitEColor.mMin.a, mInitEColor.mMax.a));
	}

	// リセット
	void Reset()
	{
		mCurrTime = 0.0f;
		mFrequencyTime = 0.0f;
	}
};
