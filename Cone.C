#include "Cone.h"

Cone::
Cone(Vector& base, double baseRadius, Vector& apex, double apexRadius):
   _base(base),
   _baseRadius(baseRadius),
   _apex(apex),
   _apexRadius(apexRadius)
{
   _dir = _apex - _base;
   _length = _dir.getMagnitude();
   
   _dir = _dir.normalise();
   
   _theta = atan((_baseRadius - _apexRadius) / _length);

   _extendedApex = _apex + _dir * (_apexRadius / tan(_theta));

   max.x = std::max(base.x, apex.x) + baseRadius;
   max.y = std::max(base.y, apex.y) + baseRadius;
   max.z = std::max(base.z, apex.z) + baseRadius;

   min.x = std::min(base.x, apex.x) - baseRadius;
   min.y = std::min(base.y, apex.y) - baseRadius;
   min.z = std::min(base.z, apex.z) - baseRadius;
}

bool Cone::
intersect(Ray& r) const
{
   /* This intersection test is taken from:
    * http://www.geometrictools.com/LibMathematics/Intersection/Wm5IntrLine3Cone3.cpp
    */
   double cosSqr = pow(cos(_theta), 2);
   
   const Vector& A = _dir;
   const Vector& D = r.dir;
   const Vector E = r.pos - _extendedApex;
   
   double c2 = pow(A.dot(D), 2) - cosSqr;
   double c1 = A.dot(D) * A.dot(E) - cosSqr * D.dot(E);
   double c0 = pow(A.dot(E), 2) - cosSqr * E.dot(E);
   
   double disc = c1 * c1 - c0 * c2;
   
   /* We ignore cases where disc == 0 / disc < 0. The former has no visible
    * impact on image quality, and the latter indicates an abscence of
    * intersections. False will be returned for both occurrences. */
   if (disc > 0)
   {
      double t0 = (-c1 - sqrt(disc)) * (1 / c2);
      double t1 = (-c1 + sqrt(disc)) * (1 / c2);

      /* Note that this object is one-sided. As such, we are only interested in
       * the first intersection that is in front of the ray origin. */
      double t_final = t0;
      if (t0 < 0)
      {
         if (t1 < 0)
         {
            return false;
         }
         else
         {
            t_final = t1;
         }
      }
      else
      {
         if (t1 < t0)
         {
            t_final = t1;
         }
      }
      
      Vector point = r.pos + r.dir * t_final;

      /* This ensures that the point is within the stated apex & base
       * coordinates. */
      if (((point - _apex).dot(A) <= 0) && ((point - _base).dot(A) >= 0))
      {
         r.intersection = point;
         r.intersected = this;
         
         /* To calculate the normal at a point, we project that point onto
          * the direction vector of the cone and add it to the apex. Now we
          * have a point on the central axis of the cone. We can simply
          * subtract this from the point on the surface to obtain the
          * normal. */
         Vector v = point - _apex;
         Vector proj = _dir * (v.dot(_dir) / _dir.dot(_dir));
         r.normal = (point - (_apex + proj)).normalise();

         return true;
      }
   }
   else
   {
      return false;
   }
}
