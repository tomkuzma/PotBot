################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/cl2000" -v28 -ml -mt -O2 --include_path="C:/Users/jbate/workspace_v10/PotBot" --include_path="C:/ti/bios_6_83_00_18/packages/ti/posix/ccs" --include_path="C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/include" --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-328165199:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-328165199-inproc

build-328165199-inproc: ../PotBot.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/ccs1110/xdctools_3_62_01_16_core/xs" --xdcpath="C:/ti/bios_6_83_00_18/packages;" xdc.tools.configuro -o configPkg -t ti.targets.C28_large -p ti.platforms.tms320x28:TMS320F28027 -r release -c "C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS" --compileOptions "-g" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-328165199 ../PotBot.cfg
configPkg/compiler.opt: build-328165199
configPkg/: build-328165199


