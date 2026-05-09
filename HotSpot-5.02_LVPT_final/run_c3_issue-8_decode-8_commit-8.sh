#--------------------------------------------------
# DEFAULT CONFIGURATION
# with LVPT
#--------------------------------------------------
# APPLU
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/applu_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid 

cp results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.steady results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.init -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/applu_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -o results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/applu_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid -grid_steady_file results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_with_LVPT.flp results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady > results/applu_with_lvpt_c3_issue-8_decode-8_commit-8.svg

#--------------------------------------------------
# BZIP2
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid 

cp results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.steady results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.init -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -o results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid -grid_steady_file results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_with_LVPT.flp results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady > results/bzip2_with_lvpt_c3_issue-8_decode-8_commit-8.svg

#--------------------------------------------------
# EQUAKE
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/equake_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid 

cp results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.steady results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.init -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/equake_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -o results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/equake_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid -grid_steady_file results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_with_LVPT.flp results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady > results/equake_with_lvpt_c3_issue-8_decode-8_commit-8.svg

#--------------------------------------------------
# GCC
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid 

cp results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.steady results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.init -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -o results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid -grid_steady_file results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_with_LVPT.flp results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady > results/gcc_with_lvpt_c3_issue-8_decode-8_commit-8.svg

#--------------------------------------------------
# GZIP
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid 

cp results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.steady results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.init -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -o results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid -grid_steady_file results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_with_LVPT.flp results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady > results/gzip_with_lvpt_c3_issue-8_decode-8_commit-8.svg

#--------------------------------------------------
# MESA
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid 

cp results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.steady results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.init -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -o results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_with_LVPT.flp -p ptraces/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.ptrace -steady_file results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.steady -model_type grid -grid_steady_file results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_with_LVPT.flp results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.grid.steady > results/mesa_with_lvpt_c3_issue-8_decode-8_commit-8.svg


#--------------------------------------------------
# DEFAULT CONFIGURATION
# without LVPT
#--------------------------------------------------
# APPLU
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/applu_without_lvpt_c3.ptrace -steady_file results/applu_without_lvpt_c3.steady -model_type grid 

cp results/applu_without_lvpt_c3.steady results/applu_without_lvpt_c3.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/applu_without_lvpt_c3.init -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/applu_without_lvpt_c3.ptrace -o results/applu_without_lvpt_c3.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/applu_without_lvpt_c3.ptrace -steady_file results/applu_without_lvpt_c3.steady -model_type grid -grid_steady_file results/applu_without_lvpt_c3.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_without_LVPT.flp results/applu_without_lvpt_c3.grid.steady > results/applu_without_lvpt_c3.svg

#--------------------------------------------------
# BZIP2
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/bzip2_without_lvpt_c3.ptrace -steady_file results/bzip2_without_lvpt_c3.steady -model_type grid 

cp results/bzip2_without_lvpt_c3.steady results/bzip2_without_lvpt_c3.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/bzip2_without_lvpt_c3.init -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/bzip2_without_lvpt_c3.ptrace -o results/bzip2_without_lvpt_c3.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/bzip2_without_lvpt_c3.ptrace -steady_file results/bzip2_without_lvpt_c3.steady -model_type grid -grid_steady_file results/bzip2_without_lvpt_c3.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_without_LVPT.flp results/bzip2_without_lvpt_c3.grid.steady > results/bzip2_without_lvpt_c3.svg

#--------------------------------------------------
# EQUAKE
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/equake_without_lvpt_c3.ptrace -steady_file results/equake_without_lvpt_c3.steady -model_type grid 

cp results/equake_without_lvpt_c3.steady results/equake_without_lvpt_c3.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/equake_without_lvpt_c3.init -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/equake_without_lvpt_c3.ptrace -o results/equake_without_lvpt_c3.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/equake_without_lvpt_c3.ptrace -steady_file results/equake_without_lvpt_c3.steady -model_type grid -grid_steady_file results/equake_without_lvpt_c3.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_without_LVPT.flp results/equake_without_lvpt_c3.grid.steady > results/equake_without_lvpt_c3.svg

#--------------------------------------------------
# GCC
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/gcc_without_lvpt_c3.ptrace -steady_file results/gcc_without_lvpt_c3.steady -model_type grid 

cp results/gcc_without_lvpt_c3.steady results/gcc_without_lvpt_c3.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gcc_without_lvpt_c3.init -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/gcc_without_lvpt_c3.ptrace -o results/gcc_without_lvpt_c3.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/gcc_without_lvpt_c3.ptrace -steady_file results/gcc_without_lvpt_c3.steady -model_type grid -grid_steady_file results/gcc_without_lvpt_c3.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_without_LVPT.flp results/gcc_without_lvpt_c3.grid.steady > results/gcc_without_lvpt_c3.svg

#--------------------------------------------------
# GZIP
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/gzip_without_lvpt_c3.ptrace -steady_file results/gzip_without_lvpt_c3.steady -model_type grid 

cp results/gzip_without_lvpt_c3.steady results/gzip_without_lvpt_c3.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/gzip_without_lvpt_c3.init -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/gzip_without_lvpt_c3.ptrace -o results/gzip_without_lvpt_c3.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/gzip_without_lvpt_c3.ptrace -steady_file results/gzip_without_lvpt_c3.steady -model_type grid -grid_steady_file results/gzip_without_lvpt_c3.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_without_LVPT.flp results/gzip_without_lvpt_c3.grid.steady > results/gzip_without_lvpt_c3.svg

#--------------------------------------------------
# MESA
#--------------------------------------------------
# Preheating
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/mesa_without_lvpt_c3.ptrace -steady_file results/mesa_without_lvpt_c3.steady -model_type grid 

cp results/mesa_without_lvpt_c3.steady results/mesa_without_lvpt_c3.init

# Generate temp. trace
./hotspot -c hotspot.config -init_file results/mesa_without_lvpt_c3.init -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/mesa_without_lvpt_c3.ptrace -o results/mesa_without_lvpt_c3.ttrace -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C3_without_LVPT.flp -p ptraces/mesa_without_lvpt_c3.ptrace -steady_file results/mesa_without_lvpt_c3.steady -model_type grid -grid_steady_file results/mesa_without_lvpt_c3.grid.steady
./grid_thermal_map.pl floorplans/ev6_C3_without_LVPT.flp results/mesa_without_lvpt_c3.grid.steady > results/mesa_without_lvpt_c3.svg