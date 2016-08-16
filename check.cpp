#include "check.h"

Check::Check()
{

}
int Check::CheckcmbModulation_1(QString _cmd){
    if(!_cmd.compare("BPSK")) return 0;
    if(!_cmd.compare("QPSK")) return 1;
    if(!_cmd.compare("0QPSK")) return 2;
    if(!_cmd.compare("8PSK")) return 3;
    if(!_cmd.compare("8QAM")) return 4;
    if(!_cmd.compare("16QAM")) return 6;
    return -1;
}
int Check::CheckcmbModulation_2(QString _cmd){
    if(!_cmd.compare("Mod")) return 1;
    if(!_cmd.compare("Demod")) return 2;
    return -1;
}

int Check::Check_Scamber(QString _cmd){
    if(!_cmd.compare("Disabled")) return 0;
    if(!_cmd.compare("Auto")) return 1;
    if(!_cmd.compare("V.35")) return 2;
    if(!_cmd.compare("Intelsat")) return 3;
    if(!_cmd.compare("Alt.V.35")) return 4;
    if(!_cmd.compare("Alt Intelsat")) return 5;
    if(!_cmd.compare("EFD")) return 6;
    if(!_cmd.compare("R-S Sync")) return 7;
    if(!_cmd.compare("IBS Sync")) return 8;
    if(!_cmd.compare("FEC Sync")) return 9;
    return -1;
}

int Check::Check_FEC_Type(QString _cmd){
    if(!_cmd.compare("Uncoded")) return 0;
    if(!_cmd.compare("Viterbi")) return 1;
    if(!_cmd.compare("TCM")) return 2;
    if(!_cmd.compare("TCP")) return 4;
    if(!_cmd.compare("LDPC")) return 5;
    if(!_cmd.compare("S-Tec")) return 6;
    return -1;
}


int Check::Check_Mod_Clock_Source(QString _cmd)
{
    if(!_cmd.compare("Internal")) return 0;
    if(!_cmd.compare("TT Clock")) return 1;
    if(!_cmd.compare("External")) return 2;
    if(!_cmd.compare("RCV Clock")) return 3;
    return -1;
}


int Check::Check_Demod_Clock_Source(QString _cmd)
{
    if(!_cmd.compare("RCV Clock")) return 0;
    if(!_cmd.compare("Internal")) return 1;
    if(!_cmd.compare("External")) return 2;
    if(!_cmd.compare("Mod Clock")) return 3;
    return -1;
}


int Check::Check_Mode(QString _cmd)
{
    if(!_cmd.compare("Disabled")) return 0;
    if(!_cmd.compare("IESS-308")) return 1;
    if(!_cmd.compare("IESS-309")) return 2;
    if(!_cmd.compare("IESS-310")) return 3;
    if(!_cmd.compare("Custom")) return 4;
    if(!_cmd.compare("CT 220/200")) return 5;
    return -1;
}


int Check::Check_depth(QString _cmd)
{
    if(!_cmd.compare("4")) return 0;
    if(!_cmd.compare("8")) return 1;
    if(!_cmd.compare("16")) return 2;
    return -1;
}


int Check::Check_Dir(QString _cmd)
{
    if(!_cmd.compare("Normal")) return 0;
    if(!_cmd.compare("Inverted")) return 1;
    return -1;
}


int Check::Check_Filter(QString _cmd)
{
    if(!_cmd.compare("IESS")) return 0;
    if(!_cmd.compare("Legacy")) return 1;
    return -1;
}


int Check::Check_Output(QString _cmd)
{
    if(!_cmd.compare("Enable")) return 1;
    if(!_cmd.compare("Disable")) return 0;
    return -1;
}


int Check::Check_Mute(QString _cmd)
{
    if(!_cmd.compare("Auto")) return 0;
    if(!_cmd.compare("Confirm")) return 1;
    if(!_cmd.compare("Manual")) return 2;
    if(!_cmd.compare("Manual & Pwr")) return 3;
    return -1;
}


int Check::Check_AUPC(QString _cmd)
{
    if(!_cmd.compare("Disable")) return 0;
    if(!_cmd.compare("Remote")) return 1;
    if(!_cmd.compare("Local")) return 2;
    return -1;
}


int Check::Check_FEC_Option(QString _cmd)
{
    if(!_cmd.compare("None")) return 0;
    if(!_cmd.compare("Normal")) return 0;
    if(!_cmd.compare("Normal")) return 0;
    if(!_cmd.compare("Advanced")) return 0;
    if(!_cmd.compare("256 Block")) return 0;
    if(!_cmd.compare("Large")) return 0;

    if(!_cmd.compare("Swap C0/C1")) return 1;
    if(!_cmd.compare("M5 Full")) return 1;
    if(!_cmd.compare("512 Block")) return 1;
    if(!_cmd.compare("Medium")) return 1;

    if(!_cmd.compare("CT")) return 2;
    if(!_cmd.compare("M5 Short")) return 2;
    if(!_cmd.compare("1K Block")) return 2;
    if(!_cmd.compare("Small")) return 2;

    if(!_cmd.compare("M5 Legacy")) return 3;
    if(!_cmd.compare("2K Block")) return 3;
    if(!_cmd.compare("XLarge")) return 3;

    if(!_cmd.compare("CT")) return 4;
    if(!_cmd.compare("4K Block")) return 4;

    if(!_cmd.compare("8K Block")) return 5;

    if(!_cmd.compare("16K Block")) return 6;
    return -1;
}


int Check::Check_FEC_Rate(QString _cmd,QString _fect, QString _feco)
{
    if(_fect=="Uncoded"){
        if(_feco=="None"){
            if(!_cmd.compare("N/A")) return 0;
        }
    }
    else if(_fect=="Viterbi"){
        if(_feco=="Normal"){
            if(!_cmd.compare("1/2")) return 0;
            if(!_cmd.compare("3/4")) return 1;
            if(!_cmd.compare("5/6")) return 2;
            if(!_cmd.compare("7/8")) return 3;
        }
        else if(_feco=="Swap C0/C1"){
            if(!_cmd.compare("1/2")) return 0;
            if(!_cmd.compare("3/4")) return 1;
            if(!_cmd.compare("5/6")) return 2;
            if(!_cmd.compare("7/8")) return 3;
        }
        else if(_feco=="CT"){
            if(!_cmd.compare("3/4")) return 0;
            if(!_cmd.compare("7/8")) return 1;
        }
    }
    else if(_fect=="TCM"){
        if(_feco=="Normal"){
            if(!_cmd.compare("2/3")) return 0;
        }
    }
    else if(_fect=="TPC"){
        if(_feco=="Advanced"){
            if(!_cmd.compare("0.453-16K")) return 0;
            if(!_cmd.compare("1/2-16K")) return 1;
            if(!_cmd.compare("1/2-4K")) return 2;
            if(!_cmd.compare("3/4-16K")) return 3;
            if(!_cmd.compare("3/4-4K")) return 4;
            if(!_cmd.compare("7/8-16K")) return 5;
            if(!_cmd.compare("7/8-4K")) return 6;
            if(!_cmd.compare("0.922-16K")) return 7;
            if(!_cmd.compare("0.950-4K")) return 8;
        }
        else if(_feco=="M5 Full"){
            if(!_cmd.compare("1/2")) return 0;
            if(!_cmd.compare("3/4")) return 1;
            if(!_cmd.compare("7/8")) return 2;
        }
        else if(_feco=="M5 Short"){
            if(!_cmd.compare("3/4")) return 0;
            if(!_cmd.compare("7/8")) return 1;
        }
        else if(_feco=="M5 Legacy"){
            if(!_cmd.compare("3/4")) return 0;
            if(!_cmd.compare("7/8")) return 1;
        }
        else if(_feco=="CT"){
            if(!_cmd.compare("5/16")) return 0;
            if(!_cmd.compare("21/44")) return 1;
            if(!_cmd.compare("3/4")) return 2;
            if(!_cmd.compare("7/8")) return 3;
            if(!_cmd.compare("0.95")) return 4;
        }
    }
    else if(_fect=="LDPC"){
        if(_feco=="256 Block" || _feco=="512 Block" || _feco=="1K Block" || _feco=="2K Block" || _feco=="4K Block" || _feco=="8K Block" || _feco=="16K Block"){
            if(!_cmd.compare("1/2")) return 0;
            if(!_cmd.compare("2/3")) return 1;
            if(!_cmd.compare("3/4")) return 2;
            if(!_cmd.compare("14/17")) return 3;
            if(!_cmd.compare("7/8")) return 4;
            if(!_cmd.compare("10/11")) return 5;
            if(!_cmd.compare("16/17")) return 6;
        }
    }

    if(!_cmd.compare("1/2")) return 0;
    if(!_cmd.compare("2/3")) return 0;

    if(!_cmd.compare("1/2")) return 0;
    if(!_cmd.compare("1/2")) return 0;

    if(!_cmd.compare("3/4")) return 1;

    if(!_cmd.compare("2/3")) return 1;
    if(!_cmd.compare("3/5")) return 1;

    if(!_cmd.compare("5/6")) return 2;

    if(!_cmd.compare("3/4")) return 2;
    if(!_cmd.compare("3/4")) return 2;

    if(!_cmd.compare("7/8")) return 3;

    if(!_cmd.compare("14/17")) return 3;
    if(!_cmd.compare("4/5")) return 3;



    if(!_cmd.compare("7/8")) return 4;
    if(!_cmd.compare("5/6")) return 4;


    if(!_cmd.compare("10/11")) return 5;
    if(!_cmd.compare("7/8")) return 5;

    if(!_cmd.compare("16/17")) return 6;
    return -1;
}
