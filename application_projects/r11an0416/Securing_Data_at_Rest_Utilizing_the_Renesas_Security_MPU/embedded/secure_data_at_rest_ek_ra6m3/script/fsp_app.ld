/*
                  Linker File for Renesas FSP
*/

INCLUDE memory_regions_app.ld

QSPI_FLASH_PRV_LENGTH = DEFINED(QSPI_FLASH_SIZE) ? ABSOLUTE(QSPI_FLASH_SIZE) : ABSOLUTE(QSPI_FLASH_LENGTH);
OSPI_DEVICE_0_PRV_LENGTH = DEFINED(OSPI_DEVICE_0_SIZE) ? ABSOLUTE(OSPI_DEVICE_0_SIZE) : ABSOLUTE(OSPI_DEVICE_0_LENGTH);
OSPI_DEVICE_1_PRV_LENGTH = DEFINED(OSPI_DEVICE_1_SIZE) ? ABSOLUTE(OSPI_DEVICE_1_SIZE) : ABSOLUTE(OSPI_DEVICE_1_LENGTH);

/* If a flat (secure) project has DEFINED RAM_NS_BUFFER_LENGTH, then emit IDAU symbols to allocate non-secure RAM. */
__RESERVE_NS_RAM = !DEFINED(PROJECT_NONSECURE) && DEFINED(RAM_NS_BUFFER_LENGTH);

RAM_NS_BUFFER_BLOCK_LENGTH = DEFINED(RAM_NS_BUFFER_LENGTH) ? ALIGN(RAM_NS_BUFFER_LENGTH, 8192) : 0;
RAM_NS_BUFFER_LENGTH = DEFINED(RAM_NS_BUFFER_LENGTH) ? RAM_NS_BUFFER_LENGTH : 0;
RAM_NS_BUFFER_START = RAM_START + RAM_LENGTH - RAM_NS_BUFFER_LENGTH;
RAM_NS_BUFFER_BLOCK_START = RAM_START + RAM_LENGTH - RAM_NS_BUFFER_BLOCK_LENGTH;

OPTION_SETTING_START_NS = 0x0100A180;

PROJECT_SECURE_OR_FLAT = !DEFINED(PROJECT_NONSECURE) && OPTION_SETTING_LENGTH;

/* Define memory regions. */
MEMORY
{
  VECTOR_TABLE (rx)      : ORIGIN = VECTOR_TABLE_START, LENGTH = VECTOR_TABLE_LENGTH
  SECURE_PROGRAM  (rx)      : ORIGIN = SECURE_PROGRAM_START, LENGTH = SECURE_PROGRAM_LENGTH
  SECURE_DATA    (rw)      : ORIGIN = SECURE_DATA_START, LENGTH = SECURE_DATA_LENGTH
  FLASH (rx)      : ORIGIN = FLASH_START, LENGTH = FLASH_LENGTH
  SECURE_RAM_PROGRAM  (rwx)       : ORIGIN = SECURE_RAM_PROGRAM_START, LENGTH = SECURE_RAM_PROGRAM_LENGTH
  SECURE_RAM (rw)      : ORIGIN = SECURE_RAM_DATA_START, LENGTH = SECURE_RAM_DATA_LENGTH
  RAM (rwx)       : ORIGIN = RAM_START, LENGTH = RAM_LENGTH
  DATA_FLASH (rx) : ORIGIN = DATA_FLASH_START, LENGTH = DATA_FLASH_LENGTH
  QSPI_FLASH (rx) : ORIGIN = QSPI_FLASH_START, LENGTH = QSPI_FLASH_PRV_LENGTH
  OSPI_DEVICE_0 (rx) : ORIGIN = OSPI_DEVICE_0_START, LENGTH = OSPI_DEVICE_0_PRV_LENGTH
  OSPI_DEVICE_1 (rx) : ORIGIN = OSPI_DEVICE_1_START, LENGTH = OSPI_DEVICE_1_PRV_LENGTH
  SDRAM (rwx)     : ORIGIN = SDRAM_START, LENGTH = SDRAM_LENGTH
  OPTION_SETTING (r): ORIGIN = OPTION_SETTING_START, LENGTH = OPTION_SETTING_LENGTH
  OPTION_SETTING_S (r): ORIGIN = OPTION_SETTING_S_START, LENGTH = OPTION_SETTING_S_LENGTH
  ID_CODE (rx)    : ORIGIN = ID_CODE_START, LENGTH = ID_CODE_LENGTH
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be DEFINED in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 *   __qspi_flash_start__
 *   __qspi_flash_end__
 *   __qspi_flash_code_size__
 *   __qspi_region_max_size__
 *   __qspi_region_start_address__
 *   __qspi_region_end_address__
 *   __ospi_device_0_start__
 *   __ospi_device_0_end__
 *   __ospi_device_0_code_size__
 *   __ospi_device_0_region_max_size__
 *   __ospi_device_0_region_start_address__
 *   __ospi_device_0_region_end_address__
 *   __ospi_device_1_start__
 *   __ospi_device_1_end__
 *   __ospi_device_1_code_size__
 *   __ospi_device_1_region_max_size__
 *   __ospi_device_1_region_start_address__
 *   __ospi_device_1_region_end_address__
 */
ENTRY(Reset_Handler)

SECTIONS
{

	.vector_table_section :
	{
        /* Even though the vector table is not 256 entries (1KB) long, we still allocate that much
         * space because ROM registers are at address 0x400 and there is very little space
         * in between. */
         __Vectors_Start = .;
		KEEP(*(.fixed_vectors*))
        KEEP(*(.application_vectors*))

		__Vectors_End = .;
		__end__ = .;

	} > VECTOR_TABLE

	__Vectors_Size = __Vectors_End - __Vectors_Start;

    .secure_text :
    {
        	__SECURE_ROM_Start = .;
		PROVIDE(__SECURE_ROM_Start = .);

		/* ROM Registers start at address 0x00000400 */
		KEEP(*(.rom_registers*))

        /* Reserving 0x100 bytes of space for ROM registers. */
		. = __SECURE_ROM_Start + 0x100;

	*(.secure_text*)

		/* .ctors */
		*(.secure_ctors)

		/* .dtors */
 		*(.secure_dtors)

		__SECURE_ROM_End = .;
		PROVIDE(__SECURE_ROM_End = .);

	} > SECURE_PROGRAM

	__e_secure_text = .;

	.secure_rodata :
 	{
 		__secure_rodata_start__ = .;

 	    . = ALIGN(4);

 		*(.secure_rodata*)

 	    . = ALIGN(4);

		/* All data end */
		__secure_rodata_end__ = .;

 	} > SECURE_DATA

 	.secure_data_section :
	{
		__secure_data_start__ = .;

		*(.secure_data*)

		. = ALIGN(4);


		/* All data end */
		__secure_data_end__ = .;

	} > SECURE_RAM AT>SECURE_DATA

    .secure_bss :
	{
		. = ALIGN(4);
		__secure_bss_start__ = .;
		*(.secure_bss*)
		. = ALIGN(4);
		__secure_bss_end__ = .;

	} > SECURE_RAM

	PROVIDE(__secure_stack = __secure_StackTop);
/* Secure application SRAM code and secure flash driver code are stored in this section. */
	.secure_sram_program :
	{
        . = ALIGN(8);
	    __secure_sram_program_start = .;
		KEEP(*(.secure_sram_program*))

		__Code_In_RAM_Start = .;
		KEEP(*(.code_in_ram*))
		__Code_In_RAM_End = .;

		__secure_sram_program_end = .;

	} > SECURE_RAM_PROGRAM AT>SECURE_PROGRAM


    .text :
    {

        __ROM_Start = .;

        *(.text*)

        KEEP(*(.version))
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)
        __usb_dev_descriptor_start_fs = .;
        KEEP(*(.usb_device_desc_fs*))
        __usb_cfg_descriptor_start_fs = .;
        KEEP(*(.usb_config_desc_fs*))
        __usb_interface_descriptor_start_fs = .;
        KEEP(*(.usb_interface_desc_fs*))
        __usb_descriptor_end_fs = .;
        __usb_dev_descriptor_start_hs = .;
        KEEP(*(.usb_device_desc_hs*))
        __usb_cfg_descriptor_start_hs = .;
        KEEP(*(.usb_config_desc_hs*))
        __usb_interface_descriptor_start_hs = .;
        KEEP(*(.usb_interface_desc_hs*))
        __usb_descriptor_end_hs = .;

        KEEP(*(.eh_frame*))

        __ROM_End = .;
    } > FLASH = 0xFF

    __Vectors_Size = __Vectors_End - __Vectors;

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
    /*
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG (__data_end__ - __data_start__)
        LONG (__etext2)
        LONG (__data2_start__)
        LONG (__data2_end__ - __data2_start__)
        __copy_table_end__ = .;
    } > FLASH
    */

    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
    /*
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        LONG (__bss2_start__)
        LONG (__bss2_end__ - __bss2_start__)
        __zero_table_end__ = .;
    } > FLASH
    */

    __etext = .;

    ___ddsc_RAM_S = ORIGIN(RAM);

    /* If DTC is used, put the DTC vector table at the start of SRAM.
       This avoids memory holes due to 1K alignment required by it. */
    .fsp_dtc_vector_table (NOLOAD) :
    {
        . = ORIGIN(RAM);
        *(.fsp_dtc_vector_table)
    } > RAM

    /* Initialized data section. */
    .data :
    {
        __data_start__ = .;
        . = ALIGN(4);

        __Code_In_RAM_Start = .;

        KEEP(*(.code_in_ram*))
        __Code_In_RAM_End = .;

        *(vtable)
        /* Don't use *(.data*) because it will place data meant for .data_flash in this section. */
        *(.data.*)
        *(.data)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(4);

        /* All data end */
        __data_end__ = .;

    } > RAM AT > FLASH


    /* TrustZone Secure Gateway Stubs Section. */
    .gnu.sgstubs : ALIGN (1024)
    {
        . = DEFINED(FLASH_NSC_START) ? ABSOLUTE(FLASH_NSC_START) : ALIGN(1024);
        ___ddsc_FLASH_C = DEFINED(FLASH_NSC_START) ? ABSOLUTE(FLASH_NSC_START) : __RESERVE_NS_RAM ? ABSOLUTE(FLASH_START + FLASH_LENGTH) : ALIGN(1024);
        _start_sg = .;
        *(.gnu.sgstubs*)
        . = ALIGN(32);
        _end_sg = .;
    } > FLASH

    ___ddsc_FLASH_N = DEFINED(FLASH_NS_START) ? ABSOLUTE(FLASH_NS_START) : __RESERVE_NS_RAM ? ABSOLUTE(FLASH_START + FLASH_LENGTH) : ALIGN(32768);

    /* Note: There are no secure/non-secure boundaries for QSPI.  These symbols are provided for the RA configuration tool. */
    ___ddsc_QSPI_FLASH_S = ORIGIN(QSPI_FLASH);

    /* QSPI_FLASH section to be downloaded via debugger */
    .qspi_flash :
    {
        __qspi_flash_start__ = .;
        KEEP(*(.qspi_flash*))
        KEEP(*(.code_in_qspi*))
        __qspi_flash_end__ = .;
    } > QSPI_FLASH
    __qspi_flash_code_size__ = __qspi_flash_end__ - __qspi_flash_start__;

    /* QSPI_FLASH non-retentive section, creates a copy in internal flash that can be copied to QSPI */
    __qspi_flash_code_addr__ = __etext + (__data_end__ - __data_start__);
    .qspi_non_retentive : AT (__qspi_flash_code_addr__)
    {
        __qspi_non_retentive_start__ = .;
        KEEP(*(.qspi_non_retentive*))
        __qspi_non_retentive_end__ = .;
    } > QSPI_FLASH
    __qspi_non_retentive_size__ = __qspi_non_retentive_end__ - __qspi_non_retentive_start__;

    __qspi_region_max_size__ = 0x4000000;   /* Must be the same as defined in MEMORY above */
    __qspi_region_start_address__ = __qspi_flash_start__;
    __qspi_region_end_address__ = __qspi_flash_start__ + __qspi_region_max_size__;

    /* Note: There are no secure/non-secure boundaries for QSPI.  These symbols are provided for the RA configuration tool. */
    ___ddsc_QSPI_FLASH_N = __qspi_non_retentive_end__;

    /* Note: There are no secure/non-secure boundaries for QSPI.  These symbols are provided for the RA configuration tool. */
    ___ddsc_OSPI_DEVICE_0_S = ORIGIN(OSPI_DEVICE_0);

    /* OSPI_DEVICE_0 section to be downloaded via debugger */
    .OSPI_DEVICE_0 :
    {
        __ospi_device_0_start__ = .;
        KEEP(*(.ospi_device_0*))
        KEEP(*(.code_in_ospi_device_0*))
        __ospi_device_0_end__ = .;
    } > OSPI_DEVICE_0
    __ospi_device_0_code_size__ = __ospi_device_0_end__ - __ospi_device_0_start__;

    /* OSPI_DEVICE_0 non-retentive section, creates a copy in internal flash that can be copied to OSPI */
    __ospi_device_0_code_addr__ = __etext + (__data_end__ - __data_start__);
    .ospi_device_0_non_retentive : AT (__ospi_device_0_code_addr__)
    {
        __ospi_device_0_non_retentive_start__ = .;
        KEEP(*(.ospi_device_0_non_retentive*))
        __ospi_device_0_non_retentive_end__ = .;
    } > OSPI_DEVICE_0
    __ospi_device_0_non_retentive_size__ = __ospi_device_0_non_retentive_end__ - __ospi_device_0_non_retentive_start__;

    __ospi_device_0_region_max_size__ = 0x8000000;   /* Must be the same as defined in MEMORY above */
    __ospi_device_0_region_start_address__ = __ospi_device_0_start__;
    __ospi_device_0_region_end_address__ = __ospi_device_0_start__ + __ospi_device_0_region_max_size__;

    /* Note: There are no secure/non-secure boundaries for OSPI.  These symbols are provided for the RA configuration tool. */
    ___ddsc_OSPI_DEVICE_0_N = __ospi_device_0_non_retentive_end__;

    /* Note: There are no secure/non-secure boundaries for OSPI.  These symbols are provided for the RA configuration tool. */
    ___ddsc_OSPI_DEVICE_1_S = ORIGIN(OSPI_DEVICE_1);

    /* OSPI_DEVICE_1 section to be downloaded via debugger */
    .OSPI_DEVICE_1 :
    {
        __ospi_device_1_start__ = .;
        KEEP(*(.ospi_device_1*))
        KEEP(*(.code_in_ospi_device_1*))
        __ospi_device_1_end__ = .;
    } > OSPI_DEVICE_1
    __ospi_device_1_code_size__ = __ospi_device_1_end__ - __ospi_device_1_start__;

    /* OSPI_DEVICE_1 non-retentive section, creates a copy in internal flash that can be copied to OSPI */
    __ospi_device_1_code_addr__ = __etext + (__data_end__ - __data_start__);
    .ospi_device_1_non_retentive : AT (__ospi_device_1_code_addr__)
    {
        __ospi_device_1_non_retentive_start__ = .;
        KEEP(*(.ospi_device_1_non_retentive*))
        __ospi_device_1_non_retentive_end__ = .;
    } > OSPI_DEVICE_1
    __ospi_device_1_non_retentive_size__ = __ospi_device_1_non_retentive_end__ - __ospi_device_1_non_retentive_start__;

    __ospi_device_1_region_max_size__ = 0x10000000;   /* Must be the same as defined in MEMORY above */
    __ospi_device_1_region_start_address__ = __ospi_device_1_start__;
    __ospi_device_1_region_end_address__ = __ospi_device_1_start__ + __ospi_device_1_region_max_size__;

    /* Note: There are no secure/non-secure boundaries for OSPI.  These symbols are provided for the RA configuration tool. */
    ___ddsc_OSPI_DEVICE_1_N = __ospi_device_1_non_retentive_end__;

    .noinit (NOLOAD):
    {
        . = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
        . = ALIGN(8);
        /* Place the FreeRTOS heap here so that the __HeapLimit calculation does not include the freertos heap. */
        KEEP(*(.heap.*))
        __noinit_end = .;
    } > RAM

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    .heap (NOLOAD):
    {
        . = ALIGN(8);
        __HeapBase = .;
        /* Place the STD heap here. */
        KEEP(*(.heap))
        __HeapLimit = .;
    } > RAM

    /* Stacks are stored in this section. */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(8);
        __StackLimit = .;
        /* Main stack */
        KEEP(*(.stack))
        __StackTop = .;
        /* Thread stacks */
        KEEP(*(.stack*))
        __StackTopAll = .;
    } > RAM

    PROVIDE(__stack = __StackTopAll);

    /* This symbol represents the end of user allocated RAM. The RAM after this symbol can be used
       at run time for things such as ThreadX memory pool allocations. */
    __RAM_segment_used_end__ = ALIGN(__StackTopAll , 4);

    /* RAM_NSC_START can be used to set a fixed address for non-secure callable RAM in secure projects.
     * If it is not specified, the address for NSC RAM is the end of RAM aligned to a 1K boundary.
     * In flat projects that require non-secure RAM, this variable is set to the start of non-secure RAM. */
    ___ddsc_RAM_C = DEFINED(RAM_NSC_START) ? ABSOLUTE(RAM_NSC_START) : __RESERVE_NS_RAM ? ABSOLUTE(RAM_NS_BUFFER_BLOCK_START) : ALIGN(__RAM_segment_used_end__, 1024);

    /* RAM_NS_START can be used to set a fixed address for non-secure RAM in secure projects or flat projects.
     * RAM_NS_BUFFER_BLOCK_LENGTH is used to allocate non-secure buffers in a flat project. If it is not
     * specified, the address for NSC RAM is the end of RAM aligned to an 8K boundary.
     * In flat projects that require non-secure RAM, this variable is set to the start of non-secure RAM. */
    ___ddsc_RAM_N = DEFINED(RAM_NS_START) ? ABSOLUTE(RAM_NS_START) : __RESERVE_NS_RAM ? ABSOLUTE(RAM_NS_BUFFER_BLOCK_START) : ALIGN(___ddsc_RAM_C, 8192);

    /* Non-secure buffers must be in non-secure RAM. This is primarily used for the EDMAC in flat projects.
     * The EDMAC is a non-secure bus master and can only access non-secure RAM. */
    .ns_buffer (NOLOAD):
    {
        /* Allocate RAM on a 32-byte boundary to help with placement of Ethernet buffers. */
        . = __RESERVE_NS_RAM ? ABSOLUTE(RAM_NS_BUFFER_START & 0xFFFFFFE0) : .;

        KEEP(*(.ns_buffer*))
    } > RAM

    /* Data flash. */
    .data_flash :
    {
        . = ORIGIN(DATA_FLASH);
        ___ddsc_DATA_FLASH_S = .;
        __Data_Flash_Start = .;
        KEEP(*(.data_flash*))
        __Data_Flash_End = .;

        ___ddsc_DATA_FLASH_N = DEFINED(DATA_FLASH_NS_START) ? ABSOLUTE(DATA_FLASH_NS_START) : __RESERVE_NS_RAM ? ABSOLUTE(DATA_FLASH_START + DATA_FLASH_LENGTH) : ALIGN(1024);
    } > DATA_FLASH

    /* Note: There are no secure/non-secure boundaries for SDRAM.  These symbols are provided for the RA configuration tool. */
    ___ddsc_SDRAM_S = ORIGIN(SDRAM);

    /* SDRAM */
    .sdram (NOLOAD):
    {
        __SDRAM_Start = .;
        KEEP(*(.sdram*))
        KEEP(*(.frame*))
        __SDRAM_End = .;
    } > SDRAM

    /* Note: There are no secure/non-secure boundaries for SDRAM.  These symbols are provided for the RA configuration tool. */
    ___ddsc_SDRAM_N = __SDRAM_End;

    /* Note: There are no secure/non-secure boundaries for ID_CODE.  These symbols are provided for the RA configuration tool. */
    ___ddsc_ID_CODE_S = ORIGIN(ID_CODE);

    .id_code(NOLOAD) :
    {
        __ID_Code_Start = .;
        KEEP(*(.id_code*))
        __ID_Code_End = .;
    } > ID_CODE

    /* Note: There are no secure/non-secure boundaries for ID_CODE.  These symbols are provided for the RA configuration tool. */
    ___ddsc_ID_CODE_N = __ID_Code_End;

    /* Symbol required for RA Configuration tool. */
    ___ddsc_OPTION_SETTING_S = ORIGIN(OPTION_SETTING);

    .option_setting(NOLOAD) :
    {
        __OPTION_SETTING_Start = .;
        KEEP(*(.option_setting_ofs0))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_Start + 0x10 : __OPTION_SETTING_Start;
        KEEP(*(.option_setting_dualsel))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_Start + 0x34 : __OPTION_SETTING_Start;
        KEEP(*(.option_setting_sas))
        __OPTION_SETTING_End = .;
    } > OPTION_SETTING = 0xFF

    /* Symbol required for RA Configuration tool. */
    ___ddsc_OPTION_SETTING_N = OPTION_SETTING_START_NS;

    .option_setting_ns(NOLOAD) :
    {
        __OPTION_SETTING_NS_Start = .;
        KEEP(*(.option_setting_ofs1))
        . = DEFINED(PROJECT_NONSECURE) ? __OPTION_SETTING_NS_Start + 0x10 : __OPTION_SETTING_NS_Start;
        KEEP(*(.option_setting_banksel))
        . = DEFINED(PROJECT_NONSECURE) ? __OPTION_SETTING_NS_Start + 0x40 : __OPTION_SETTING_NS_Start;
        KEEP(*(.option_setting_bps0))
        . = DEFINED(PROJECT_NONSECURE) ? __OPTION_SETTING_NS_Start + 0x44 : __OPTION_SETTING_NS_Start;
        KEEP(*(.option_setting_bps1))
        . = DEFINED(PROJECT_NONSECURE) ? __OPTION_SETTING_NS_Start + 0x48 : __OPTION_SETTING_NS_Start;
        KEEP(*(.option_setting_bps2))
        . = DEFINED(PROJECT_NONSECURE) ? __OPTION_SETTING_NS_Start + 0x60 : __OPTION_SETTING_NS_Start;
        KEEP(*(.option_setting_pbps0))
        . = DEFINED(PROJECT_NONSECURE) ? __OPTION_SETTING_NS_Start + 0x64 : __OPTION_SETTING_NS_Start;
        KEEP(*(.option_setting_pbps1))
        . = DEFINED(PROJECT_NONSECURE) ? __OPTION_SETTING_NS_Start + 0x68 : __OPTION_SETTING_NS_Start;
        KEEP(*(.option_setting_pbps2))
        __OPTION_SETTING_NS_End = .;
    } > OPTION_SETTING = 0xFF

    /* Symbol required for RA Configuration tool. */
    ___ddsc_OPTION_SETTING_S_S = ORIGIN(OPTION_SETTING_S);

    .option_setting_s(NOLOAD) :
    {
        __OPTION_SETTING_S_Start = .;
        KEEP(*(.option_setting_ofs1_sec))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x10 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_banksel_sec))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x40 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_bps_sec0))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x44 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_bps_sec1))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x48 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_bps_sec2))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x60 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_pbps_sec0))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x64 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_pbps_sec1))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x68 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_pbps_sec2))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x80 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_ofs1_sel))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0x90 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_banksel_sel))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0xC0 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_bps_sel0))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0xC4 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_bps_sel1))
        . = PROJECT_SECURE_OR_FLAT ? __OPTION_SETTING_S_Start + 0xC8 : __OPTION_SETTING_S_Start;
        KEEP(*(.option_setting_bps_sel2))
        __OPTION_SETTING_S_End = .;
    } > OPTION_SETTING_S = 0xFF

    /* Symbol required for RA Configuration tool. */
    ___ddsc_OPTION_SETTING_S_N = __OPTION_SETTING_S_End;
}
