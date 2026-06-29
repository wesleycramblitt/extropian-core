#pragma once

namespace exd::core::units {

// Angle conversion
double radians_to_degrees(double rad);
double degrees_to_radians(double deg);

// LBM force computation for rectangular duct Poiseuille flow
double f_from_u_rectangular_duct(double width, double height,
                                  double rho, double nu, double target_u);

} // namespace exd::core::units
