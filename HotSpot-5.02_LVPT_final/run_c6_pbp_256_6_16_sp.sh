#--------------------------------------------------
# DEFAULT CONFIGURATION
# with secondary path
# with LVPT
#--------------------------------------------------
# APPLU
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/applu_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/applu_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid 

cp results/applu_with_lvpt_c6_perceptron_256_6_16_sp.steady results/applu_with_lvpt_c6_perceptron_256_6_16_sp.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/applu_with_lvpt_c6_perceptron_256_6_16_sp.init -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/applu_with_lvpt_c6_perceptron_256_6_16.ptrace -o results/applu_with_lvpt_c6_perceptron_256_6_16_sp.ttrace -model_secondary 1 -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/applu_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/applu_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid -grid_steady_file results/applu_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady
./grid_thermal_map.pl floorplans/ev6_C6_with_LVPT.flp results/applu_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady > results/applu_with_lvpt_c6_perceptron_256_6_16_sp.svg

#--------------------------------------------------
# BZIP2
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid 

cp results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.steady results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.init -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c6_perceptron_256_6_16.ptrace -o results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.ttrace -model_secondary 1 -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid -grid_steady_file results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady
./grid_thermal_map.pl floorplans/ev6_C6_with_LVPT.flp results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady > results/bzip2_with_lvpt_c6_perceptron_256_6_16_sp.svg

#--------------------------------------------------
# EQUAKE
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/equake_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/equake_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid 

cp results/equake_with_lvpt_c6_perceptron_256_6_16_sp.steady results/equake_with_lvpt_c6_perceptron_256_6_16_sp.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/equake_with_lvpt_c6_perceptron_256_6_16_sp.init -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/equake_with_lvpt_c6_perceptron_256_6_16.ptrace -o results/equake_with_lvpt_c6_perceptron_256_6_16_sp.ttrace -model_secondary 1 -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/equake_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/equake_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid -grid_steady_file results/equake_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady
./grid_thermal_map.pl floorplans/ev6_C6_with_LVPT.flp results/equake_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady > results/equake_with_lvpt_c6_perceptron_256_6_16_sp.svg

#--------------------------------------------------
# GCC
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/gcc_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid 

cp results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.steady results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.init -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/gcc_with_lvpt_c6_perceptron_256_6_16.ptrace -o results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.ttrace -model_secondary 1 -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/gcc_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid -grid_steady_file results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady
./grid_thermal_map.pl floorplans/ev6_C6_with_LVPT.flp results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady > results/gcc_with_lvpt_c6_perceptron_256_6_16_sp.svg

#--------------------------------------------------
# GZIP
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/gzip_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid 

cp results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.steady results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.init -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/gzip_with_lvpt_c6_perceptron_256_6_16.ptrace -o results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.ttrace -model_secondary 1 -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/gzip_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid -grid_steady_file results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady
./grid_thermal_map.pl floorplans/ev6_C6_with_LVPT.flp results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady > results/gzip_with_lvpt_c6_perceptron_256_6_16_sp.svg

#--------------------------------------------------
# MESA
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/mesa_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid 

cp results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.steady results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.init -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/mesa_with_lvpt_c6_perceptron_256_6_16.ptrace -o results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.ttrace -model_secondary 1 -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C6_with_LVPT.flp -p ptraces/mesa_with_lvpt_c6_perceptron_256_6_16.ptrace -steady_file results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.steady -model_secondary 1 -model_type grid -grid_steady_file results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady
./grid_thermal_map.pl floorplans/ev6_C6_with_LVPT.flp results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.grid.steady > results/mesa_with_lvpt_c6_perceptron_256_6_16_sp.svg