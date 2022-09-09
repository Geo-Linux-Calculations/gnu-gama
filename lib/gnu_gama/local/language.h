#ifndef GNU_gama_local_language_header_file_h
#define GNU_gama_local_language_header_file_h

/* !!! DO NOT EDIT !!!  created by slovnikar 1.16 */

namespace GNU_gama { namespace local {

enum gama_language { en, ca, cz, du, es, fi, fr, hu, ru, ua, zh };
void set_gama_language(gama_language);

extern const char* T_GKF_bad_angle;
extern const char* T_GKF_bad_attribute_angle_dev;
extern const char* T_GKF_bad_attribute_direction_dev;
extern const char* T_GKF_bad_attribute_distance_dev;
extern const char* T_GKF_bad_azimuth;
extern const char* T_GKF_bad_coordinate_x;
extern const char* T_GKF_bad_coordinate_y;
extern const char* T_GKF_bad_direction;
extern const char* T_GKF_bad_distance;
extern const char* T_GKF_bad_height;
extern const char* T_GKF_bad_height_diff;
extern const char* T_GKF_bad_instrument_reflector_height;
extern const char* T_GKF_bad_instuament_reflector_height;
extern const char* T_GKF_bad_network_configuration_unknown_parameter;
extern const char* T_GKF_bad_orientation_angle;
extern const char* T_GKF_bad_vector_data;
extern const char* T_GKF_bad_zangle;
extern const char* T_GKF_coordinate_x_is_not_defined;
extern const char* T_GKF_coordinate_y_is_not_defined;
extern const char* T_GKF_coordinates_without_covariance_matrix;
extern const char* T_GKF_cov_dim_differs_from_number_of_coordinates;
extern const char* T_GKF_cov_dim_differs_from_number_of_vectors;
extern const char* T_GKF_cov_mat_bad_band_width;
extern const char* T_GKF_cov_mat_bad_dim;
extern const char* T_GKF_cov_mat_bad_dim_not_enough_elements;
extern const char* T_GKF_cov_mat_bad_dim_too_many_elements;
extern const char* T_GKF_cov_mat_bad_element;
extern const char* T_GKF_cov_mat_missing_band_width;
extern const char* T_GKF_cov_mat_missing_dim;
extern const char* T_GKF_covariance_matrix_is_not_positive_definite;
extern const char* T_GKF_e01a_illegal_tag;
extern const char* T_GKF_e01b_after_tag;
extern const char* T_GKF_error_on_reading_of_absolute_terms_tolerance;
extern const char* T_GKF_error_on_reading_of_confidence_probability;
extern const char* T_GKF_error_on_reading_of_epoch;
extern const char* T_GKF_error_on_reading_of_standard_deviation;
extern const char* T_GKF_illegal_standard_deviation;
extern const char* T_GKF_illegal_text;
extern const char* T_GKF_illegal_value_of_gama_xml_version;
extern const char* T_GKF_missing_forepoint_id;
extern const char* T_GKF_missing_from_ID;
extern const char* T_GKF_missing_left_forepoint_id;
extern const char* T_GKF_missing_observed_value;
extern const char* T_GKF_missing_point_ID;
extern const char* T_GKF_missing_right_forepoint_id;
extern const char* T_GKF_missing_standpoint_id;
extern const char* T_GKF_missing_tag_network;
extern const char* T_GKF_missing_to_ID;
extern const char* T_GKF_multiple_definition_of_xy_in_tag_point;
extern const char* T_GKF_multiple_definition_of_z_in_tag_point;
extern const char* T_GKF_must_start_with_gama_xml;
extern const char* T_GKF_no_observations_after_cov_mat;
extern const char* T_GKF_point_must_define_xy_andor_z_inside_tag_coordinates;
extern const char* T_GKF_undefined_attribute_of_angle;
extern const char* T_GKF_undefined_attribute_of_azimuth;
extern const char* T_GKF_undefined_attribute_of_coordinates;
extern const char* T_GKF_undefined_attribute_of_cov_mat;
extern const char* T_GKF_undefined_attribute_of_direction;
extern const char* T_GKF_undefined_attribute_of_distance;
extern const char* T_GKF_undefined_attribute_of_gama_xml;
extern const char* T_GKF_undefined_attribute_of_height_differences;
extern const char* T_GKF_undefined_attribute_of_obs;
extern const char* T_GKF_undefined_attribute_of_points;
extern const char* T_GKF_undefined_attribute_of_points_observations;
extern const char* T_GKF_undefined_attribute_of_slopedist;
extern const char* T_GKF_undefined_attribute_of_vectors;
extern const char* T_GKF_undefined_attribute_of_zangle;
extern const char* T_GKF_undefined_height_type;
extern const char* T_GKF_undefined_point_type;
extern const char* T_GKF_undefined_value_of_attribute;
extern const char* T_GKF_vectors_without_covariance_matrix;
extern const char* T_GKF_wrong_type_of_standard_deviation;
extern const char* T_GaMa_Adjustment_of_geodetic_network;
extern const char* T_GaMa_Approximate_coordinates_replaced;
extern const char* T_GaMa_During_statistical_analysis_we_work;
extern const char* T_GaMa_Free_network;
extern const char* T_GaMa_Free_network_defect_is;
extern const char* T_GaMa_General_solution_parameters;
extern const char* T_GaMa_Given_network_configuration_can_not_be_adjusted;
extern const char* T_GaMa_Maximal_decrease_of_m0;
extern const char* T_GaMa_Maximal_normalized_residual;
extern const char* T_GaMa_No_network_points_defined;
extern const char* T_GaMa_No_observations_available;
extern const char* T_GaMa_No_points_available;
extern const char* T_GaMa_No_unknowns_defined;
extern const char* T_GaMa_Number_of_linearization_iterations;
extern const char* T_GaMa_Observatios_with_outlying_absolute_terms_removed;
extern const char* T_GaMa_Ratio_empirical_to_apriori;
extern const char* T_GaMa_Review_of_fixed_points;
extern const char* T_GaMa_abstrm_Review_of_outlying_abs_terms;
extern const char* T_GaMa_abstrm_header1;
extern const char* T_GaMa_abstrm_header2;
extern const char* T_GaMa_adjobs_Adjusted_observations;
extern const char* T_GaMa_adjobs_adjusted;
extern const char* T_GaMa_adjobs_header1;
extern const char* T_GaMa_adjobs_observed;
extern const char* T_GaMa_adjobs_value;
extern const char* T_GaMa_adjunk_Review_of_unknowns_bearings;
extern const char* T_GaMa_adjunk_Review_of_unknowns_coordidantes;
extern const char* T_GaMa_adjunk_Review_of_unknowns_heights;
extern const char* T_GaMa_adjunk_constrained;
extern const char* T_GaMa_adjunk_header1;
extern const char* T_GaMa_adjunk_header2;
extern const char* T_GaMa_adjunk_header3;
extern const char* T_GaMa_adjunk_header4;
extern const char* T_GaMa_adjunk_header5;
extern const char* T_GaMa_adjunk_header6;
extern const char* T_GaMa_adjunk_mean_position_error_average;
extern const char* T_GaMa_adjunk_mean_position_error_maximal;
extern const char* T_GaMa_adjunk_mean_position_error_on_point;
extern const char* T_GaMa_adjustment_iteration;
extern const char* T_GaMa_angle;
extern const char* T_GaMa_approx_Review_of_approximate_coordinates;
extern const char* T_GaMa_approx_computed_coordinates;
extern const char* T_GaMa_approx_given_coordinates;
extern const char* T_GaMa_approx_header1;
extern const char* T_GaMa_approx_observations;
extern const char* T_GaMa_approx_separator;
extern const char* T_GaMa_approx_total;
extern const char* T_GaMa_azimuth;
extern const char* T_GaMa_detected_singular_variables;
extern const char* T_GaMa_direction;
extern const char* T_GaMa_distance;
extern const char* T_GaMa_errell_header1;
extern const char* T_GaMa_errell_header2;
extern const char* T_GaMa_errell_review_of_mean_errors_and_error_ellipses;
extern const char* T_GaMa_exception_1;
extern const char* T_GaMa_exception_2a;
extern const char* T_GaMa_exception_2b;
extern const char* T_GaMa_from_equals_to;
extern const char* T_GaMa_genpar_Maximal_studentized_residual;
extern const char* T_GaMa_genpar_critical_value;
extern const char* T_GaMa_genpar_doesnt_exceed;
extern const char* T_GaMa_genpar_exceeds;
extern const char* T_GaMa_genpar_for_observation_ind;
extern const char* T_GaMa_genpar_on_significance_level;
extern const char* T_GaMa_gpar1_adjusted_coordinates;
extern const char* T_GaMa_gpar1_angles;
extern const char* T_GaMa_gpar1_computed_heights;
extern const char* T_GaMa_gpar1_computed_points;
extern const char* T_GaMa_gpar1_constrained_coordinates;
extern const char* T_GaMa_gpar1_coordinates;
extern const char* T_GaMa_gpar1_directions;
extern const char* T_GaMa_gpar1_distances;
extern const char* T_GaMa_gpar1_equations;
extern const char* T_GaMa_gpar1_fixed_coordinates;
extern const char* T_GaMa_gpar1_fixed_heights;
extern const char* T_GaMa_gpar1_fixed_points;
extern const char* T_GaMa_gpar1_leveling_diffs;
extern const char* T_GaMa_gpar1_obs_total;
extern const char* T_GaMa_gpar1_observed_coords;
extern const char* T_GaMa_gpar1_points_total;
extern const char* T_GaMa_gpar1_redundancy;
extern const char* T_GaMa_gpar1_s_dists;
extern const char* T_GaMa_gpar1_total;
extern const char* T_GaMa_gpar1_z_angles;
extern const char* T_GaMa_gpar2_bearings;
extern const char* T_GaMa_gpar2_constrained_heights;
extern const char* T_GaMa_gpar2_constrained_points;
extern const char* T_GaMa_gpar2_network_defect;
extern const char* T_GaMa_gpar2_number_of_unknowns;
extern const char* T_GaMa_inconsistent_coordinates_and_angles;
extern const char* T_GaMa_index_type_point;
extern const char* T_GaMa_internal_program_error;
extern const char* T_GaMa_interval;
extern const char* T_GaMa_interval_contains;
extern const char* T_GaMa_interval_doesnt_contain;
extern const char* T_GaMa_levell;
extern const char* T_GaMa_linearization;
extern const char* T_GaMa_m0_angles;
extern const char* T_GaMa_m0_apriori;
extern const char* T_GaMa_m0_directions;
extern const char* T_GaMa_m0_dirs_angs;
extern const char* T_GaMa_m0_distances;
extern const char* T_GaMa_m0_empirical;
extern const char* T_GaMa_missing_coordinates;
extern const char* T_GaMa_network_description;
extern const char* T_GaMa_network_not_connected;
extern const char* T_GaMa_not_enough_constrained_points;
extern const char* T_GaMa_padding_obs;
extern const char* T_GaMa_point;
extern const char* T_GaMa_reduced_Review_of_reduced_observations;
extern const char* T_GaMa_reduced_Review_of_reduced_observations_to_ellipsoid;
extern const char* T_GaMa_reduced_header1;
extern const char* T_GaMa_reduced_observed;
extern const char* T_GaMa_reduced_reduced;
extern const char* T_GaMa_reduced_to_ellipsoid_header1;
extern const char* T_GaMa_reduced_value;
extern const char* T_GaMa_resobs_Outlying_observations;
extern const char* T_GaMa_resobs_Review_of_residuals_analysis_obs;
extern const char* T_GaMa_resobs_condition_number;
extern const char* T_GaMa_resobs_header1;
extern const char* T_GaMa_resobs_mc_critical;
extern const char* T_GaMa_resobs_mc_max;
extern const char* T_GaMa_resobs_mc_max_critical;
extern const char* T_GaMa_resobs_no_control;
extern const char* T_GaMa_resobs_normality_test;
extern const char* T_GaMa_resobs_weak_control;
extern const char* T_GaMa_s_distance;
extern const char* T_GaMa_solution_ended_with_error;
extern const char* T_GaMa_standpoint;
extern const char* T_GaMa_statan_with_apriori_standard_deviation;
extern const char* T_GaMa_statan_with_confidence_level;
extern const char* T_GaMa_statan_with_empirical_standard_deviation;
extern const char* T_GaMa_target;
extern const char* T_GaMa_tstlin_Differences;
extern const char* T_GaMa_tstlin_Test_of_linearization_error;
extern const char* T_GaMa_tstlin_header1;
extern const char* T_GaMa_tstlin_header_value;
extern const char* T_GaMa_tstlin_obs_r_diff;
extern const char* T_GaMa_version;
extern const char* T_GaMa_x;
extern const char* T_GaMa_xdiff;
extern const char* T_GaMa_y;
extern const char* T_GaMa_ydiff;
extern const char* T_GaMa_z;
extern const char* T_GaMa_z_angle;
extern const char* T_GaMa_zdiff;
extern const char* T_HTML_adjusted;
extern const char* T_HTML_adjusted_observations;
extern const char* T_HTML_approximate;
extern const char* T_HTML_cc;
extern const char* T_HTML_confidence;
extern const char* T_HTML_correction;
extern const char* T_HTML_d;
extern const char* T_HTML_e_obs_adj;
extern const char* T_HTML_f_proc;
extern const char* T_HTML_g;
extern const char* T_HTML_m;
extern const char* T_HTML_md;
extern const char* T_HTML_mean_error_ellipse;
extern const char* T_HTML_mg;
extern const char* T_HTML_mm;
extern const char* T_HTML_mmcc;
extern const char* T_HTML_mmss;
extern const char* T_HTML_mp;
extern const char* T_HTML_mxy;
extern const char* T_HTML_observations;
extern const char* T_HTML_observed;
extern const char* T_HTML_points;
extern const char* T_HTML_r;
extern const char* T_HTML_rejected;
extern const char* T_HTML_rstud;
extern const char* T_HTML_ss;
extern const char* T_HTML_stddev_confi;
extern const char* T_HTML_value;
extern const char* T_HTML_value_d;
extern const char* T_HTML_value_g;
extern const char* T_IE_internal_error;
extern const char* T_LN_rm_huge_cov_xy;
extern const char* T_LN_rm_huge_cov_xyz;
extern const char* T_LN_rm_huge_cov_z;
extern const char* T_LN_rm_missing_xy;
extern const char* T_LN_rm_missing_xyz;
extern const char* T_LN_rm_missing_z;
extern const char* T_LN_rm_removed_points;
extern const char* T_LN_rm_singular_xy;
extern const char* T_LN_rm_singular_z;
extern const char* T_LN_undefined_confidence_level;
extern const char* T_LN_undefined_type_of_actual_sigma;
extern const char* T_POBS_bad_data;
extern const char* T_POBS_zero_or_negative_distance;
extern const char* T_POBS_zero_or_negative_slope_distance;
extern const char* T_POBS_zero_or_negative_zenith_angle;

}}

#endif
