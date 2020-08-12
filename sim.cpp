#include<iostream>
#include<sstream>
#include<string>
#include<fstream>
using namespace::std;
string bin_hex( string s) // converts a binary string to a hexadecimal string
{
    string ans = "";
    for(int i = 0; 4 * i < s.length(); i++)
    {
        string tmp = "";
        for( int j = 4 * i; j <= 4*i + 3; j++)
            tmp += s[j];
        if( tmp == "0000")
            ans += "0";
        else if( tmp == "0001")
            ans += "1";
        else if( tmp == "0010")
            ans += "2";
        else if( tmp == "0011")
            ans += "3";
        else if( tmp == "0100")
            ans += "4";
        else if( tmp == "0101")
            ans += "5";
        else if( tmp == "0110")
            ans += "6";
        else if( tmp == "0111")
            ans += "7";
        else if( tmp == "1000")
            ans += "8";
        else if( tmp == "1001")
            ans += "9";
        else if( tmp == "1010")
            ans += "A";
        else if( tmp == "1011")
            ans += "B";
        else if( tmp == "1100")
            ans += "C";
        else if( tmp == "1101")
            ans += "D";
        else if( tmp == "1110")
            ans += "E";
        else if( tmp == "1111")
            ans += "F";
    }
    return ans;
}
void itos_2(int num, string* s) // converts an integer number to a binary string
{
    int l = (*s).length(), i = 1;
    for(int j = 0; j < l; j++)
        (*s)[j] = '0';
    while( num != 0 && l >= i)
    {
        if(num % 2 == 1)
            (*s)[l - i] = '1';
        i++;
        num = num / 2;
    }
}
int main()
{
// definition
    // registers and memory as strings
        string AR = "000000000000", PC = "000000000000", DR = "0000000000000000", AC = "0000000000000000", INPR = "00000000", IR = "0000000000000000",
            TR = "0000000000000000", OUTR = "00000000", M[4096] = "0000000000000000", SC = "0";
    // signals and flip flops as integers
        int I = 0; // indirect address flip flop
        int S = 1; // start flip flop
        int E = 0; // carry flip flop
        int R = 0; // interrupt flip flop
        int IEN = 0; // input enable
        int FGI = 0, FGO = 0; // I/O flags
        int T[16] = {0}; // clock counter T0...T15
        int D[8] = {0}; // decoded signals D0...D7
    // clock counter
        int clk = 0;
    // memory counter
        int m = 0;
    // initial memory address
        int ini = 0;
// assumptions
    PC = "000100000000"; // PC <- 100
    ini = stoi(PC, nullptr, 2);
// read memory from file
    string line;
    ifstream myfile ("binary.txt");
    if( myfile.is_open())
    {
        m = ini;
        while( getline ( myfile, line))
        {
            M[m] = line;
            string tmp = "000000000000";
            itos_2(m, &tmp);
            cout << bin_hex(tmp) << "  :" << bin_hex(line) << endl;
            m++;
        }
        cout << "****************" << endl;
        myfile.close();
    } // close file
    else
        cout << "ERROR : unable to open the file" << endl;
// start
    SC = "0";
    IEN = 0;
    R = 0;
    while( S == 1)
    {
        // program cycle
            R = (~(T[0]))*(~(T[1]))*(~(T[2]))*IEN*(FGI + FGO) > 0; // T0'T1'T2'(IEN)(FGI + FGO) : R <- 1
            if( R == 1)
            // interrupt cycle
            {
                // T0 = 1
                    for(int i = 0; i <= 15; i++)
                        T[i] = 0;
                    T[0] = 1;
                    // TR <- PC
                        TR = "0000";
                        TR += PC;
                    // AR <- 0
                        itos_2( 0, &AR);
                // T1 = 1
                    T[0] = 0;
                    T[1] = 1;
                    // PC <- 0
                        itos_2( 0, &PC);
                    // M[AR] <- TR
                        int i_AR = stoi( AR, nullptr, 2);
                        M[i_AR] = TR;
                // T2 = 1
                    T[1] = 0;
                    T[2] = 1;
                    // SC <- 0
                        SC = "0";
                    // R <- 0
                        R = 0;
                    // IEN <- 0
                        IEN = 0;
                    // PC <- PC + 1
                        int i_PC = stoi(PC, nullptr, 2);
                        i_PC = i_PC + 1;
                        itos_2( i_PC, &PC);
            }
            else
            // instruction cycle
            {
                // Instruction Fetch
                    // T0 = 1
                        for(int i = 0; i <= 15; i++)
                            T[i] = 0;
                        T[0] = 1;
                        // AR <- PC
                            AR = PC;
                    // T1 = 1
                        T[0] = 0;
                        T[1] = 1;
                        // PC <- PC + 1
                            int i_PC = stoi(PC, nullptr, 2);
                            i_PC = i_PC + 1;
                            itos_2( i_PC, &PC);
                        // IR <- M[AR]
                            int i_AR = stoi(AR, nullptr, 2);
                            IR = M[i_AR];
                // Instruction Decode
                    // T2 = 1
                        T[1] = 0;
                        T[2] = 1;
                        // I <- IR[15]
                            I = (IR[0] == '1');
                        // AR <- IR[0...11]
                            for(int k = 0; k <= 11; k++)
                                AR[k] = IR[k + 4];
                        // D0...D7 <- Decode IR[12-14]
                            string s_tmp = "";
                            for(int k = 1; k <= 3; k++)
                                s_tmp += IR[k];
                            int i_tmp = stoi( s_tmp, nullptr, 2);
                            for(int i = 1; i <= 7; i++)
                                D[i] = 0;
                            D[i_tmp] = 1;
                // Instruction Execution
                    // T3 = 1
                        T[2] = 0;
                        T[3] = 1;
                        if( D[7] == 1)
                        // Register or I/O
                        {
                            if( I == 1)
                            // input-output instruction
                            {
                                // SC <- 0
                                    SC = "0";
                                if( IR[4] == '1') // INP : input character to AC
                                {
                                    // AC[0...7] <- INPR;
                                        for(int i = 0; i <= 7; i++)
                                            AC[i + 8] = INPR[i];
                                    // FGI <- 0
                                        FGI = 0;
                                }
                                else if( IR[5] == '1') // OUT : output character from AC
                                {
                                    // OUTR <- AC[0...7];
                                        for(int i = 0; i <= 7; i++)
                                            OUTR = AC[i + 8];
                                    // FGO <- 0
                                        FGO = 0;
                                }
                                else if( IR[6] == '1') // SKI : skip on input flag
                                {
                                    if( FGI == 1)
                                    // PC = PC + 1
                                        int i_PC = stoi(PC, nullptr, 2);
                                        i_PC = i_PC + 1;
                                        itos_2( i_PC, &PC);
                                }
                                else if( IR[7] == '1') // SKO : skip on output flag
                                {
                                    if( FGO == 1)
                                    // PC = PC + 1
                                        int i_PC = stoi(PC, nullptr, 2);
                                        i_PC = i_PC + 1;
                                        itos_2( i_PC, &PC);
                                }
                                else if( IR[8] == '1') // ION : interrupt On
                                {
                                    // IEN <- 1
                                        IEN = 1;
                                }
                                else if( IR[9] == '1') // IOF : interrupt Off
                                {
                                    // IEN <- 0
                                        IEN = 0;
                                }
                                else cout << "ERROR : invalid input instruction" << endl;
                            }
                            else
                            // register-reference instruction
                            {
                                // SC <- 0
                                    SC = "0";
                                if( IR[4] == '1') // CLA : clear AC
                                {
                                    // AC <- 0
                                        itos_2( 0, &AC);
                                }
                                else if( IR[5] == '1') // CLE : clear E
                                {
                                    // E <- 0
                                        E = 0;
                                }
                                else if( IR[6] == '1') // CMS : complement AC
                                {
                                    // AC <- (AC)'
                                        int i_AC = stoi(AC, nullptr, 2);
                                        i_AC = ~ i_AC;
                                        itos_2( i_AC, &AC);
                                }
                                else if( IR[7] == '1') // CME : complement E
                                {
                                    // E <- E'
                                        E = ~E;
                                }
                                else if( IR[8] == '1') // CIR : circulate right AC and E
                                {
                                    int tmp = E;
                                    // E <- AC[0]
                                        if( AC[15] == '1')
                                            E = 1;
                                        else
                                            E = 0;
                                    // AC <- shr AC
                                        int i_AC = stoi( AC, nullptr, 2);
                                        i_AC = i_AC >> 1;
                                        itos_2( i_AC, &AC);
                                    // AC[15] <- E
                                        if( tmp == 1)
                                            AC[0] = '1';
                                        else
                                            AC[0] = '0';
                                }
                                else if( IR[9] == '1') // CIL : circulate left AC and E
                                {
                                    int tmp = E;
                                    // E <- AC[15]
                                        if( AC[0] == 1)
                                            E = '1';
                                        else
                                            E = '0';
                                    // AC <- shl AC
                                        int i_AC = stoi( AC, nullptr, 2);
                                        i_AC = i_AC << 1;
                                        itos_2( i_AC, &AC);
                                    // AC[0] <- E
                                        if( tmp == 1)
                                            AC[15] = '1';
                                        else
                                            AC[15] = '0';
                                }
                                else if( IR[10] == '1') // INC : increment AC
                                {
                                    // AC <- AC + 1
                                        int i_AC = stoi(AC , nullptr, 2);
                                        i_AC = i_AC + 1;
                                        itos_2( i_AC, &AC);
                                }
                                else if( IR[11] == '1') // SPA : skip next instruction if AC positive
                                {
                                    // if(AC[15] = 0)
                                    if( AC[0] == '0')
                                        // PC <- PC + 1
                                        {
                                            int i_PC = stoi(PC , nullptr, 2);
                                            i_PC = i_PC + 1;
                                            itos_2( i_PC, &PC);
                                        }
                                }
                                else if( IR[12] == '1') // SNA : skip next instruction if AC negative
                                {
                                    // if(AC[15] = 1)
                                    if( AC[0] == '1')
                                        // PC <- PC + 1
                                        {
                                            int i_PC = stoi(PC , nullptr, 2);
                                            i_PC = i_PC + 1;
                                            itos_2( i_PC, &PC);
                                        }
                                }
                                else if( IR[13] == '1') // SZA : skip next instruction if AC zero
                                {
                                    // if(AC = 0)
                                    if( stoi(AC , nullptr, 2) == 0)
                                        // PC <- PC + 1
                                        {
                                            int i_PC = stoi(PC , nullptr, 2);
                                            i_PC = i_PC + 1;
                                            itos_2( i_PC, &PC);
                                        }
                                }
                                else if( IR[14] == '1') // SZE : skip next instruction if E zero
                                {
                                    // if(E = 0)
                                    if( E == 0)
                                        // PC <- PC + 1
                                        {
                                            int i_PC = stoi(PC , nullptr, 2);
                                            i_PC = i_PC + 1;
                                            itos_2( i_PC, &PC);
                                        }
                                }
                                else if( IR[15] == '1') // HLT : halt computer
                                {
                                    // S <- 0
                                        S = 0;
                                }
                                else cout << "ERROR : invalid input instruction" << endl;
                            }
                        }
                        else
                        // memory-reference instruction
                        {
                            if( I == 1)
                            // indirect addressing
                            {
                                // AR <- M[AR]
                                    int i_AR = stoi(AR, nullptr, 2);
                                    AR = M[i_AR];
                            }
                            else
                            // direct addressing
                            {
                                // Nothing
                                ;
                            }
                            if( D[0] == 1) // AND : And memory word to AC
                            {
                                // T[4] = 1
                                    T[3] = 0;
                                    T[4] = 1;
                                    // DR <- M[AR]
                                        int i_AR = stoi(AR, nullptr, 2);
                                        DR = M[i_AR];
                                // T[5] = 1
                                    T[4] = 0;
                                    T[5] = 1;
                                    // AC <- AC ^ DR
                                        int i_DR = stoi(DR, nullptr, 2);
                                        int i_AC = stoi(AC, nullptr, 2);
                                        int tmp = i_DR & i_AC;
                                        itos_2( tmp, &AC);
                                    // SC <- 0
                                        SC = "0";
                            }
                            else if( D[1] == 1) // ADD : Add memory word to AC
                            {
                                // T[4] = 1
                                    T[3] = 0;
                                    T[4] = 1;
                                    // DR <- M[AR]
                                        int i_AR = stoi(AR, nullptr, 2);
                                        DR = M[i_AR];
                                // T[5] = 1
                                    T[4] = 0;
                                    T[5] = 1;
                                    // AC <- AC + DR
                                        int i_DR = stoi(DR, nullptr, 2);
                                        int i_AC = stoi(AC, nullptr, 2);
                                        long int tmp = i_DR + i_AC;
                                        itos_2( tmp, &AC);
                                    // E <- Cout
                                        E = tmp >> 16;
                                    // SC <- 0
                                        SC = "0";
                            }
                            else if( D[2] == 1) // LDA : Load memory word to AC
                            {
                                // T[4] = 1
                                    T[3] = 0;
                                    T[4] = 1;
                                    // DR <- M[AR]
                                        int i_AR = stoi(AR, nullptr, 2);
                                        DR = M[i_AR];
                                // T[5] = 1
                                    T[4] = 0;
                                    T[5] = 1;
                                    // AC <- DR
                                        AC = DR;
                                    // SC <- 0
                                        SC = "0";
                            }
                            else if( D[3] == 1) // STA : store content of AC to memory
                            {
                                // T[4] = 1
                                    T[3] = 0;
                                    T[4] = 1;
                                    // M[AR] <- AC
                                        int i_AR = stoi(AR, nullptr, 2);
                                        M[i_AR] = AC;
                                    // SC <- 0
                                        SC = "0";
                            }
                            else if( D[4] == 1) // BUN : branch unconditionally
                            {
                                // T[4] = 1
                                    T[3] = 0;
                                    T[4] = 1;
                                    // PC <- AR
                                        PC = AR;
                                    // SC <- 0
                                        SC = "0";
                            }
                            else if( D[5] == 1) // BSA : branch and save return address
                            {
                                // T[4] = 1
                                    T[3] = 0;
                                    T[4] = 1;
                                    // M[AR] <- PC
                                        int i_AR = stoi(AR, nullptr, 2);
                                        M[i_AR] = PC;
                                    // AR <- AR + 1
                                        i_AR = i_AR + 1;
                                        itos_2( i_AR, &AR);
                                // T[5] = 1
                                    T[4] = 0;
                                    T[5] = 1;
                                    // PC <- AR
                                        PC = AR;
                                    // SC <- 0
                                        SC = "0";

                            }
                            else if( D[6] == 1) // ISZ : increment and skip if zero
                            {
                                // T[4] = 1
                                    T[3] = 0;
                                    T[4] = 1;
                                    // DR <- M[AR]
                                        int i_AR = stoi(AR, nullptr, 2);
                                        DR = M[i_AR];
                                // T[5] = 1
                                    T[4] = 0;
                                    T[5] = 1;
                                    // DR = DR + 1;
                                        int i_DR = stoi(DR, nullptr, 2);
                                        i_DR = i_DR + 1;
                                        itos_2( i_DR, &DR);
                                // T[6] = 1
                                    T[5] = 0;
                                    T[6] = 1;
                                    // M[AR] <- DR
                                        M[i_AR] = DR;
                                    // if( DR = 0)
                                        if( DR == "0000000000000000")
                                        // PC <- PC + 1
                                        {

                                            int i_PC = stoi( PC, nullptr, 2);
                                            i_PC = i_PC + 1;
                                            itos_2( i_PC, &PC);
                                        }
                                        else
                                        // SC <- 0
                                            SC = "0";
                            }
                            else cout << "ERROR : invalid input instruction" << endl;
                        }

            }
//            cout << "AR   : " << bin_hex(AR) << endl;
//            cout << "PC   : " << bin_hex(PC) << endl;
//            cout << "DR   : " << bin_hex(DR) << endl;
//            cout << "AC   : " << bin_hex(AC) << endl;
//            cout << "IR   : " << bin_hex(IR) << endl;
//            cout << "TR   : " << bin_hex(TR) << endl;
//            cout << "INPR : " << bin_hex(INPR) << endl;
//            cout << "OUTR : " << bin_hex(OUTR) << endl;
//            cout << "E    : " << E << endl;
            int t = 0;
            for( t = 0; t <= 15; t++ )
                if( T[t] == 1)
                    break;
//            cout << "Clock Cycle : T" << t << endl;
            clk += t;
//            cout << "Total Clocks: " << clk << endl;
//            cout << "****************" << endl;
    }// end program
    for( int i = ini; i <= m; i++)
    {
        string tmp = "000000000000";
        itos_2(i, &tmp);
        cout << bin_hex(tmp) << "  :" << bin_hex(M[i]) << endl;
    }
    cout << "****************" << endl;
    cout << "AR   : " << bin_hex(AR) << endl;
    cout << "PC   : " << bin_hex(PC) << endl;
    cout << "DR   : " << bin_hex(DR) << endl;
    cout << "AC   : " << bin_hex(AC) << endl;
    cout << "IR   : " << bin_hex(IR) << endl;
    cout << "TR   : " << bin_hex(TR) << endl;
    cout << "INPR : " << bin_hex(INPR) << endl;
    cout << "OUTR : " << bin_hex(OUTR) << endl;
    cout << "E    : " << E << endl;
    cout << "Total Clocks: " << clk << endl;
    cout << "****************" << endl;
}
