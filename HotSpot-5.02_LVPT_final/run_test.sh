#--------------------------------------------------
# DEFAULT CONFIGURATION
# with secondary path
# with LVPT
#--------------------------------------------------
# APPLU
#--------------------------------------------------
# Preheating
#./hotspot -c hotspot.config -f floorplans/ev6_C1_with_LVPT.flp -p ptraces/applu_with_lvpt_c1.ptrace -steady_file results/applu_with_lvpt_c1_sp.steady -model_secondary 1 -model_type grid 

#cp results/applu_with_lvpt_c1_sp.steady results/applu_with_lvpt_c1_sp.init

# Generate temp. trace
#./hotspot -c hotspot.config -init_file results/applu_with_lvpt_c1_sp.init -f floorplans/ev6_C1_with_LVPT.flp -p ptraces/applu_with_lvpt_c1.ptrace -o results/applu_with_lvpt_c1_sp.ttrace -model_secondary 1 -model_type grid 

# Thermal map
./hotspot -c hotspot.config -f floorplans/ev6_C1_with_LVPT.flp -p ptraces/applu_with_lvpt_c1.ptrace -steady_file results/applu_with_lvpt_c1_sp.steady -model_secondary 1 -model_type grid -grid_steady_file results/applu_with_lvpt_c1_sp.grid.steady
./grid_thermal_map.pl floorplans/ev6_C1_with_LVPT.flp results/applu_with_lvpt_c1_sp.grid.steady > results/applu_with_lvpt_c1_sp.svg
