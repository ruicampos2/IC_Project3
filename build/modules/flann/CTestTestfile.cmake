# CMake generated Testfile for 
# Source directory: /home/ruicampos/Desktop/IC/IC_Project2/opencv/modules/flann
# Build directory: /home/ruicampos/Desktop/IC/IC_Project2/build/modules/flann
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_flann "/home/ruicampos/Desktop/IC/IC_Project2/build/bin/opencv_test_flann" "--gtest_output=xml:opencv_test_flann.xml")
set_tests_properties(opencv_test_flann PROPERTIES  LABELS "Main;opencv_flann;Accuracy" WORKING_DIRECTORY "/home/ruicampos/Desktop/IC/IC_Project2/build/test-reports/accuracy" _BACKTRACE_TRIPLES "/home/ruicampos/Desktop/IC/IC_Project2/opencv/cmake/OpenCVUtils.cmake;1769;add_test;/home/ruicampos/Desktop/IC/IC_Project2/opencv/cmake/OpenCVModule.cmake;1375;ocv_add_test_from_target;/home/ruicampos/Desktop/IC/IC_Project2/opencv/cmake/OpenCVModule.cmake;1133;ocv_add_accuracy_tests;/home/ruicampos/Desktop/IC/IC_Project2/opencv/modules/flann/CMakeLists.txt;2;ocv_define_module;/home/ruicampos/Desktop/IC/IC_Project2/opencv/modules/flann/CMakeLists.txt;0;")
