#include <cmath>
#include <cstdio>

namespace exd::core::units {

// ── SI unit conversion helpers ──

double radians_to_degrees(double rad) { return rad * 180.0 / M_PI; }
double degrees_to_radians(double deg) { return deg * M_PI / 180.0; }

// LBM force computation for rectangular duct
double f_from_u_rectangular_duct(double width, double height,
                                  double rho, double nu, double target_u) {
    // Pressure-driven Poiseuille flow force for a rectangular duct
    double dh = (2.0 * width * height) / (width + height); // hydraulic diameter
    double re = target_u * dh / nu;
    double f_darcy = 64.0 / re; // laminar friction factor approximation
    double dp = f_darcy * (1.0 / dh) * 0.5 * rho * target_u * target_u;
    return dp / rho;
}

} // namespace exd::core::units
