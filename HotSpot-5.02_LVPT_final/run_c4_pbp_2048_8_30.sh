#--------------------------------------------------
# DEFAULT CONFIGURATION
# with LVPT
#--------------------------------------------------
# APPLU
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/applu_with_lvpt_c4_perceptron.ptrace -steady_file results/applu_with_lvpt_c4_perceptron.steady -model_type grid 

cp results/applu_with_lvpt_c4_perceptron.steady results/applu_with_lvpt_c4_perceptron.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/applu_with_lvpt_c4_perceptron.init -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/applu_with_lvpt_c4_perceptron.ptrace -o results/applu_with_lvpt_c4.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/applu_with_lvpt_c4_perceptron.ptrace -steady_file results/applu_with_lvpt_c4_perceptron.steady -model_type grid -grid_steady_file results/applu_with_lvpt_c4.grid.steady
./grid_thermal_map.pl floorplans/ev6_C4_with_LVPT.flp results/applu_with_lvpt_c4.grid.steady > results/applu_with_lvpt_c4.svg

#--------------------------------------------------
# BZIP2
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c4_perceptron.ptrace -steady_file results/bzip2_with_lvpt_c4_perceptron.steady -model_type grid 

cp results/bzip2_with_lvpt_c4_perceptron.steady results/bzip2_with_lvpt_c4_perceptron.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/bzip2_with_lvpt_c4_perceptron.init -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c4_perceptron.ptrace -o results/bzip2_with_lvpt_c4.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c4_perceptron.ptrace -steady_file results/bzip2_with_lvpt_c4_perceptron.steady -model_type grid -grid_steady_file results/bzip2_with_lvpt_c4.grid.steady
./grid_thermal_map.pl floorplans/ev6_C4_with_LVPT.flp results/bzip2_with_lvpt_c4.grid.steady > results/bzip2_with_lvpt_c4.svg

#--------------------------------------------------
# EQUAKE
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/equake_with_lvpt_c4_perceptron.ptrace -steady_file results/equake_with_lvpt_c4_perceptron.steady -model_type grid 

cp results/equake_with_lvpt_c4_perceptron.steady results/equake_with_lvpt_c4_perceptron.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/equake_with_lvpt_c4_perceptron.init -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/equake_with_lvpt_c4_perceptron.ptrace -o results/equake_with_lvpt_c4.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/equake_with_lvpt_c4_perceptron.ptrace -steady_file results/equake_with_lvpt_c4_perceptron.steady -model_type grid -grid_steady_file results/equake_with_lvpt_c4.grid.steady
./grid_thermal_map.pl floorplans/ev6_C4_with_LVPT.flp results/equake_with_lvpt_c4.grid.steady > results/equake_with_lvpt_c4.svg

#--------------------------------------------------
# GCC
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/gcc_with_lvpt_c4_perceptron.ptrace -steady_file results/gcc_with_lvpt_c4_perceptron.steady -model_type grid 

cp results/gcc_with_lvpt_c4_perceptron.steady results/gcc_with_lvpt_c4_perceptron.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gcc_with_lvpt_c4_perceptron.init -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/gcc_with_lvpt_c4_perceptron.ptrace -o results/gcc_with_lvpt_c4.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/gcc_with_lvpt_c4_perceptron.ptrace -steady_file results/gcc_with_lvpt_c4_perceptron.steady -model_type grid -grid_steady_file results/gcc_with_lvpt_c4.grid.steady
./grid_thermal_map.pl floorplans/ev6_C4_with_LVPT.flp results/gcc_with_lvpt_c4.grid.steady > results/gcc_with_lvpt_c4.svg

#--------------------------------------------------
# GZIP
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/gzip_with_lvpt_c4_perceptron.ptrace -steady_file results/gzip_with_lvpt_c4_perceptron.steady -model_type grid 

cp results/gzip_with_lvpt_c4_perceptron.steady results/gzip_with_lvpt_c4_perceptron.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gzip_with_lvpt_c4_perceptron.init -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/gzip_with_lvpt_c4_perceptron.ptrace -o results/gzip_with_lvpt_c4.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/gzip_with_lvpt_c4_perceptron.ptrace -steady_file results/gzip_with_lvpt_c4_perceptron.steady -model_type grid -grid_steady_file results/gzip_with_lvpt_c4.grid.steady
./grid_thermal_map.pl floorplans/ev6_C4_with_LVPT.flp results/gzip_with_lvpt_c4.grid.steady > results/gzip_with_lvpt_c4.svg

#--------------------------------------------------
# MESA
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/mesa_with_lvpt_c4_perceptron.ptrace -steady_file results/mesa_with_lvpt_c4_perceptron.steady -model_type grid 

cp results/mesa_with_lvpt_c4_perceptron.steady results/mesa_with_lvpt_c4_perceptron.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/mesa_with_lvpt_c4_perceptron.init -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/mesa_with_lvpt_c4_perceptron.ptrace -o results/mesa_with_lvpt_c4.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C4_with_LVPT.flp -p ptraces/mesa_with_lvpt_c4_perceptron.ptrace -steady_file results/mesa_with_lvpt_c4_perceptron.steady -model_type grid -grid_steady_file results/mesa_with_lvpt_c4.grid.steady
./grid_thermal_map.pl floorplans/ev6_C4_with_LVPT.flp results/mesa_with_lvpt_c4.grid.steady > results/mesa_with_lvpt_c4.svg

