#include <winerror.h>
#include "api.h" 
#include <stdio.h>
#include <time.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <iterator>
#include <iosfwd>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <Windows.h>
#include "mttapi.h"
#include "reg.h"
#include "fnc.h"



using namespace MuTest;

double icc_max=0, icc_average=0;



int hello_world(API * api) {
	api->info_message("Hello World!");
	return EXIT_SUCCESS;
}
/*
int force_fail(API * api) {
	api->set_result_bin_name("FAIL_BIN");
	return EXIT_SUCCESS;
}
*/
int Set_Relay_VCC(API * api){
   
        
//      UBIT_NONE = 0, //   does nothing
//       UBIT_0 = 1, //                 UBIT #0
//       UBIT_1 = 2, //                 UBIT #1
//       UBIT_2 = 4, //                 UBIT #2
//       UBIT_3 = 8, //                 UBIT #3
//       UBIT_4 = 16, //                UBIT #4
//       UBIT_5 = 32, //                UBIT #5
//       UBIT_6 = 64, //                UBIT #6
//       UBIT_7 = 128, //     UBIT #7
//       UBIT_8 = 256, //     UBIT #8
//       UBIT_ALL = 511 //       All Utility Bits
        
    api->set_loadboard_ubits( UBIT_0, UBIT_1 + UBIT_2 + UBIT_3 + UBIT_4 + UBIT_5 + UBIT_6 + UBIT_7 + UBIT_8);
             Sleep(500); // setup delay, 500ms minimum !
    return EXIT_SUCCESS;
}
int Set_Relay_VCC_PE(API * api){
   
        
//      UBIT_NONE = 0, //   does nothing
//       UBIT_0 = 1, //                 UBIT #0
//       UBIT_1 = 2, //                 UBIT #1
//       UBIT_2 = 4, //                 UBIT #2
//       UBIT_3 = 8, //                 UBIT #3
//       UBIT_4 = 16, //                UBIT #4
//       UBIT_5 = 32, //                UBIT #5
//       UBIT_6 = 64, //                UBIT #6
//       UBIT_7 = 128, //     UBIT #7
//       UBIT_8 = 256, //     UBIT #8
//       UBIT_ALL = 511 //       All Utility Bits
        
    api->set_loadboard_ubits( UBIT_1 + UBIT_2 + UBIT_5 + UBIT_7  ,UBIT_0 + UBIT_3 + UBIT_4 + UBIT_6 + UBIT_8);
             Sleep(50); // setup delay, 500ms minimum !   
    return EXIT_SUCCESS;
}


//****************************************************************************************************************************************************

int Reset_Relay(API * api){
        //int set_loadboard_ubits(unsigned long ON, unsigned long OFF);
        api->set_loadboard_ubits( UBIT_NONE, UBIT_ALL);
                Sleep(50); // setup delay, 500ms minimum !
        return EXIT_SUCCESS;
}

int P5V_ON(API * api){
           // this will set +5V and +15V on and -15V off. 
    api->Set_loadboard_supplies(LBD_P5V, LBD_P15V+ LBD_M15V, 10000); 
    
        return EXIT_SUCCESS;
}


int Relay_OPEN(API * api){
           // this will set +5V and +15V on and -15V off. 
    api->set_loadboard_ubits( UBIT_NONE, UBIT_ALL);
            Sleep(500); // setup delay, 500ms minimum !
    
        return EXIT_SUCCESS;
}




int Compute_ICC(API * api, const char * Test_Name,const char * DC_BLOCK_NAME,int Nbr_measure,int act_datalog) {
    
    int i=0;
    double * measure;
    double Result[Nbr_measure];
    double Result_average, tmp=0,result_max=-1000;
    double HiLimit, LoLimit;
    
    api->set_trace_mode(TR_NONE);
    api->Execute_test(Test_Name);
     for (i = 0; i < Nbr_measure; i++)
    {
         api->Get_dctest_values( DC_BLOCK_NAME, "VCC",i, THE_MEASURE, &measure, THE_END);
         Result[i]= measure[0];
         tmp= measure[0] + tmp;                 // Somme des mesures
    }
    for   (i = 0; i < Nbr_measure; i++ )   // detection de la valeur max
    {
      if  (Result[i]>result_max)
       {
           result_max=Result[i];
       }
    }   
    api->Get_dctest_values( DC_BLOCK_NAME, "VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);         
    api->set_trace_mode(TR_ALL);
    Result_average=tmp/Nbr_measure;
    icc_max=result_max;  
    icc_average=Result_average;
    if (act_datalog==1) // activation datalog
    {
        api->info_message("Test:%s:%e A %e %e" ,Test_Name ,Result_average, LoLimit, HiLimit);
        api->datalog_print("Test:%s:%e A %e %e", Test_Name ,Result_average, LoLimit, HiLimit);

        if (Result_average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    }
        return EXIT_SUCCESS;            
}



int ICCH1_Measure(API * api ) {
    double ICCH1=0,ICCH1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCH_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCH1_MULTI","ICCH_MULTI",40,0);
    ICCH1 = icc_max;
    ICCH1_Average = icc_average;
    api->info_message("Test:ICCH1max:%e A %e %e"  ,ICCH1, LoLimit, HiLimit);
    api->info_message("Test:ICCH1:%e A %e %e"  ,ICCH1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCH1:%e A %e %e" ,ICCH1_Average, LoLimit, HiLimit);

        if (ICCH1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



int ICCH1BA_Measure(API * api ) {
    double ICCH1=0,ICCH1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC1H_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC1HBA_MULTI","ICC1H_MULTI",40,0);
    ICCH1 = icc_max;
    ICCH1_Average = icc_average;
    api->info_message("Test:ICC1HBAmax:%e A %e %e"  ,ICCH1, LoLimit, HiLimit);
    api->info_message("Test:ICC1HBA:%e A %e %e"  ,ICCH1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICC1HBA:%e A %e %e" ,ICCH1_Average, LoLimit, HiLimit);

        if (ICCH1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



int ICCH1AB_Measure(API * api ) {
    double ICCH1=0,ICCH1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC1H_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC1HAB_MULTI","ICC1H_MULTI",40,0);
    ICCH1 = icc_max;
    ICCH1_Average = icc_average;
    api->info_message("Test:ICCH1HABmax:%e A %e %e"  ,ICCH1, LoLimit, HiLimit);
    api->info_message("Test:ICCH1HAB:%e A %e %e"  ,ICCH1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCH1HAB:%e A %e %e" ,ICCH1_Average, LoLimit, HiLimit);

        if (ICCH1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCL1BA_Measure(API * api ) {
    // double ICCL1=0;
    double ICCL1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC1L_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC1LBA_MULTI","ICC1L_MULTI",40,0);
    // ICCL1 = icc_max;
    ICCL1_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICCL1BA:%e A %e %e"  ,ICCL1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCL1BA:%e A %e %e" ,ICCL1_Average, LoLimit, HiLimit);

        if (ICCL1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



int ICCL1AB_Measure(API * api ) {
    // double ICCL1=0;
    double ICCL1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC1L_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC1LAB_MULTI","ICC1L_MULTI",40,0);
    // ICCL1 = icc_max;
    ICCL1_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICC1LAB:%e A %e %e"  ,ICCL1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICC1LAB:%e A %e %e" ,ICCL1_Average, LoLimit, HiLimit);

        if (ICCL1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}




int ICCH2BA_Measure(API * api ) {
    // double ICCH2=0;
    double ICCH2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC2H_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC2HBA_MULTI","ICC2H_MULTI",40,0);
    // ICCH2 = icc_max;
    ICCH2_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICC2HBA:%e A %e %e"  ,ICCH2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICC2HBA:%e A %e %e" ,ICCH2_Average, LoLimit, HiLimit);

        if (ICCH2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



int ICCH2AB_Measure(API * api ) {
    // double ICCH2=0;
    double ICCH2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC2H_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC2HAB_MULTI","ICC2H_MULTI",40,0);
    // ICCH2 = icc_max;
    ICCH2_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICC2HAB:%e A %e %e"  ,ICCH2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICC2HAB:%e A %e %e" ,ICCH2_Average, LoLimit, HiLimit);

        if (ICCH2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCL2BA_Measure(API * api ) {
    // double ICCL2=0;
    double ICCL2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC2L_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC2LBA_MULTI","ICC2L_MULTI",40,0);
    // ICCL2 = icc_max;
    ICCL2_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICC2LBA:%e A %e %e"  ,ICCL2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICC2LBA:%e A %e %e" ,ICCL2_Average, LoLimit, HiLimit);

        if (ICCL2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



int ICCL2AB_Measure(API * api ) {
    // double ICCL2=0;
    double ICCL2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICC2L_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICC2LAB_MULTI","ICC2L_MULTI",40,0);
    // ICCL2 = icc_max;
    ICCL2_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICC2LAB:%e A %e %e"  ,ICCL2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICC2LAB:%e A %e %e" ,ICCL2_Average, LoLimit, HiLimit);

        if (ICCL2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}




int ICCH2_Measure(API * api ) {
    // double ICCH2=0;
    double ICCH2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCH_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCH2_MULTI","ICCH_MULTI",40,0);
    // ICCH2 = icc_max;
    ICCH2_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICCH2:%e A %e %e"  ,ICCH2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCH2:%e A %e %e" ,ICCH2_Average, LoLimit, HiLimit);

        if (ICCH2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



int ICCL1_Measure(API * api ) {
    // double ICCL1=0;
    double ICCL1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCL_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCL1_MULTI","ICCL_MULTI",40,0);
    // ICCL1 = icc_max;
    ICCL1_Average = icc_average;
    // api->info_message("Test:ICCL:%e A %e %e"  ,ICCL, LoLimit, HiLimit);
    api->info_message("Test:ICCL1:%e A %e %e"  ,ICCL1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCL1:%e A %e %e" ,ICCL1_Average, LoLimit, HiLimit);

        if (ICCL1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCL2_Measure(API * api ) {
    // double ICCL2=0;
    double ICCL2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCL_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCL2_MULTI","ICCL_MULTI",40,0);
    // ICCL2 = icc_max;
    ICCL2_Average = icc_average;
    // api->info_message("Test:ICCL:%e A %e %e"  ,ICCL, LoLimit, HiLimit);
    api->info_message("Test:ICCL2:%e A %e %e"  ,ICCL2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCL2:%e A %e %e" ,ICCL2_Average, LoLimit, HiLimit);

        if (ICCL2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCZ1_Measure(API * api ) {
    // double ICCZ1=0;
    double ICCZ1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCZ_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCZ1_MULTI","ICCZ_MULTI",40,0);
    // ICCZ1 = icc_max;
    ICCZ1_Average = icc_average;
    // api->info_message("Test:ICCZ:%e A %e %e"  ,ICCZ, LoLimit, HiLimit);
    api->info_message("Test:ICCD1:%e A %e %e"  ,ICCZ1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCD1:%e A %e %e" ,ICCZ1_Average, LoLimit, HiLimit);

        if (ICCZ1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCZ2_Measure(API * api ) {
    // double ICCZ2=0;
    double ICCZ2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCZ_MULTI","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCZ2_MULTI","ICCZ_MULTI",40,0);
    // ICCZ2 = icc_max;
    ICCZ2_Average = icc_average;
    // api->info_message("Test:ICCZ:%e A %e %e"  ,ICCZ, LoLimit, HiLimit);
    api->info_message("Test:ICCD2:%e A %e %e"  ,ICCZ2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCD2:%e A %e %e" ,ICCZ2_Average, LoLimit, HiLimit);

        if (ICCZ2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}

int ICCH1HR_Measure(API * api ) {
    // double ICCH1=0;
    double ICCH1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCH_MULTI_HI_RANGE","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCH1_MULTI_HI_RANGE","ICCH_MULTI_HI_RANGE",40,0);
    // ICCH1 = icc_max;
    ICCH1_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICCH1:%e A %e %e"  ,ICCH1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCH1:%e A %e %e" ,ICCH1_Average, LoLimit, HiLimit);

        if (ICCH1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCH2HR_Measure(API * api ) {
    // double ICCH2=0;
    double ICCH2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCH_MULTI_HI_RANGE","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCH2_MULTI_HI_RANGE","ICCH_MULTI_HI_RANGE",40,0);
    // ICCH2 = icc_max;
    ICCH2_Average = icc_average;
    // api->info_message("Test:ICCH:%e A %e %e"  ,ICCH, LoLimit, HiLimit);
    api->info_message("Test:ICCH2:%e A %e %e"  ,ICCH2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCH2:%e A %e %e" ,ICCH2_Average, LoLimit, HiLimit);

        if (ICCH2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



int ICCL1HR_Measure(API * api ) {
    // double ICCL1=0;
    double ICCL1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCL_MULTI_HI_RANGE","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCL1_MULTI_HI_RANGE","ICCL_MULTI_HI_RANGE",40,0);
    // ICCL1 = icc_max;
    ICCL1_Average = icc_average;
    // api->info_message("Test:ICCL:%e A %e %e"  ,ICCL, LoLimit, HiLimit);
    api->info_message("Test:ICCL1:%e A %e %e"  ,ICCL1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCL1:%e A %e %e" ,ICCL1_Average, LoLimit, HiLimit);

        if (ICCL1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCL2HR_Measure(API * api ) {
    // double ICCL2=0;
    double ICCL2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCL_MULTI_HI_RANGE","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCL2_MULTI_HI_RANGE","ICCL_MULTI_HI_RANGE",40,0);
    // ICCL2 = icc_max;
    ICCL2_Average = icc_average;
    // api->info_message("Test:ICCL:%e A %e %e"  ,ICCL, LoLimit, HiLimit);
    api->info_message("Test:ICCL2:%e A %e %e"  ,ICCL2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCL2:%e A %e %e" ,ICCL2_Average, LoLimit, HiLimit);

        if (ICCL2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCZ1HR_Measure(API * api ) {
    // double ICCZ1=0;
    double ICCZ1_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCZ_MULTI_HI_RANGE","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCZ1_MULTI_HI_RANGE","ICCZ_MULTI_HI_RANGE",40,0);
    // ICCZ1 = icc_max;
    ICCZ1_Average = icc_average;
    // api->info_message("Test:ICCZ:%e A %e %e"  ,ICCZ, LoLimit, HiLimit);
    api->info_message("Test:ICCZ1:%e A %e %e"  ,ICCZ1_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCZ1:%e A %e %e" ,ICCZ1_Average, LoLimit, HiLimit);

        if (ICCZ1_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}


int ICCZ2HR_Measure(API * api ) {
    // double ICCZ2=0;
    double ICCZ2_Average=0;
    double HiLimit, LoLimit;
    
    api->Get_dctest_values( "ICCZ_MULTI_HI_RANGE","VCC",0, THE_LIMIT_HIGH, &HiLimit, THE_LIMIT_LOW, &LoLimit, THE_END);
    Compute_ICC(api, "ICCZ2_MULTI_HI_RANGE","ICCZ_MULTI_HI_RANGE",40,0);
    // ICCZ2 = icc_max;
    ICCZ2_Average = icc_average;
    // api->info_message("Test:ICCZ:%e A %e %e"  ,ICCZ, LoLimit, HiLimit);
    api->info_message("Test:ICCZ2:%e A %e %e"  ,ICCZ2_Average, LoLimit, HiLimit);
    api->datalog_print("Test:ICCZ2:%e A %e %e" ,ICCZ2_Average, LoLimit, HiLimit);

        if (ICCZ2_Average <= HiLimit)     /// PASS ?
        {
              api->set_result_bin_name("PASS_BIN");
        }
        else  //FAIL
        {
              api->set_result_bin_name("FAIL_BIN"); 
         }
    return EXIT_SUCCESS;
}



long id; // port handler
int open_connection(API * api) {
     const char * name = api->Get_tester_ip();
     mtt_open(name, 2010, &id, "USER CODE");    // open port
     return EXIT_SUCCESS;
}

int Reload_Pmupp_Offsets(API * api) {
     unsigned long data_read;

mtt_wf(id, 0x4, 0xFFFF);                   // talking to all tester channel. WR48 and MD1632 will not be involved in the transfer
mtt_wf(id, LVL_PMUPPVIOS, 0x0);           // PMUPP V offset is set to 0.0v (default value)
mtt_wf(id, LVL_PMUPPIIOS, 0x2800);        // PMUPP I offset is set to default value
mtt_rf(id, LVL_PMUPPIIOS, &data_read);     // Flushes buffer
return EXIT_SUCCESS;
}
