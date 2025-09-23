/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates some of the capabilities of the EK and FSP by using multiple peripherals.
        The project initializes the QSPI , LPM , SLCD and GPIO modules to create an interactive user demo.

        Once programmed the user can control the board by button presses on the board and see regularly updated board 
        configuration on the kit information screen.
        On the attached Segment LCD panel the user can interact with the application, please read the following document
        d020234_11_s01_v0100_r20qs0076eg0100 Evaluation Kit for RA4C1 Microcontroller Group EK-RA4C1 Quick Start Guide

2. Software Requirements:
        Renesas Flexible Software Package (FSP): Version 6.1.0
        e2 studio: Version 2025-07
	Terminal Console Application: Tera Term or a similar application
        GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware settings for the project:
        Please configure the Jumper settings as described in the Quick Start Guide.
	Link to access Quick Start Guide - https://www.renesas.com/en/document/qsg/ek-ra4c1-quick-start-guide

        Configuration switch (SW4) settings
            SW4-1 SW4-2 SW4-3 SW4-4 SW4-5 SW4-6 SW4-7 SW4-8
             OFF   OFF   OFF   OFF   OFF   OFF   OFF   OFF
        
4. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.