#pragma once
#include "framework/math/Matrix4x4.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    template<typename _T>
    class BoundingBox
    {
    public:
        BoundingBox(const _T& min, const _T& max);
        ~BoundingBox();

        void setMin(const _T& min);
        const _T& getMin() const;
        void setMax(const _T& max);
        const _T& getMax() const;
        void set(const _T& min, const _T& max);
        _T getDimension() const;
        _T getCenter() const;
		bool isInside(_T coordinate) const;

		void cropToBorder(const BoundingBox<_T>& border );

		bool operator==(const BoundingBox<_T>& other);
		bool operator!=(const BoundingBox<_T>& other);

    private:
        _T min;
        _T max;
    };

    template<typename _T>
    std::ostream& operator<<(std::ostream& stream, const BoundingBox<_T>& boundingBox);
	
typedef BoundingBox<Vec3f> BoundingBox3f;
	typedef BoundingBox<Vec3ui> BoundingBox3ui;
	typedef BoundingBox<Vec3i> BoundingBox3i;

    BoundingBox3f getTransform(const BoundingBox3f& base, const Matrix4x4& transform);
}