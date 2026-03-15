void msgT_H(char *msg,int T_H_int,int T_H_dec,char Type)
{
    if(T_H_int < 0)
    {
        msg[0]='-';
        T_H_int = -T_H_int;
    } 
    else
    {
        msg[0]='+';
    }

    msg[1]=(T_H_int/10)+48;
    msg[2]=(T_H_int%10)+48;
    msg[3]=',';
    msg[4]=(T_H_dec/10)+48;
    msg[5]=(T_H_dec%10)+48;

    switch(Type)
    {
        case 'T':
        case 't':
            msg[6]='C';
            break;

        case 'H':
        case 'h':
            msg[6]='%';
            break;

        default:
            msg[6]='?';   // sécurité
            break;
    }

    msg[7]='\r';
    msg[8]='\n';
    msg[9]='\0';
}


