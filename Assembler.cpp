#include<iostream>
#include<sstream>
#include<string>
#include<fstream>
using namespace::std;
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
string operand_address(int i, int PC, string* label, string target)
{
    for(int l = 0; l <= i; l++)
    {
        if(label[l] == target)
        {
            int tmp = l + PC - 1; // the address of the operand
            string s_tmp = "000000000000";
            itos_2( tmp, &s_tmp);
            return s_tmp;
        }
    }
}
int main()
{
    int error_flag = 0; // checks if an error has occurred
    int i = 0; // counter for the lines of the text file
// reads lines from text file
    string line[1000] = "", label[1000] = "", opcode[1000] = "", target[1000] = "", comment[1000] = "";
    int I[1000] = {0}; // indirect
    ifstream myfile ("mult.txt");
    if( myfile.is_open())
    {
        while( getline ( myfile, line[i]))
        {
            int j = 0; // counter for the characters of the line
            // Label field
                if( line[i][1] == ',' || line[i][2] == ',' || line[i][3] == ',' )
                {
                    label[i] +=  line[i][0];
                    j++;
                    while(line[i][j] != ',')
                    {
                        label[i] +=  line[i][j];
                        j++;
                    }
                    // makes sure next character is a space
                        j++;
                        if( line[i][j] != ' ')
                        {
                            cout << "ERROR : invalid input format" << endl;
                            error_flag = 1;
                            break;
                        }
                    j++;
                }
            // Instruction field
                // Opcode
                    for(int k = j; k <= j+2; k++)
                    {
                        opcode[i] +=  line[i][k];
                    }
                    j += 3;
                // Target
                    if( j < line[i].length() && line[i][j] == ' ' && line[i][j+1] != '/')
                    {
                        j++;
                        while( j < line[i].length() && line[i][j] != ' ')
                        {
                            target[i] += line[i][j];
                            j++;
                        }
                    }
                // Indirect
                    if( j < line[i].length() && line[i][j] == ' ' && line[i][j+1] == 'I')
                    {
                        I[i] = 1;
                        j += 2;
                    }
                // Comment
                    if( j < line[i].length() && line[i][j] == ' ' && line[i][j+1] == '/')
                    {
                        j += 2;
                        while( j < line[i].length() )
                        {
                            comment[i] += line[i][j];
                            j++;
                        }
                    }
            // next line
                i++;
                if( i > 999)
                {
                    cout << "ERROR : source text file has more than 1000 lines" << endl;
                    error_flag = 1;
                    break;
                }
        }
        myfile.close();
    } // close file
    else
    {
        cout << "ERROR : unable to open the file" << endl;
        error_flag = 1;
    }
    // show the assembly text file
//    for( int k = 0; k <= i; k++)
//    {
//        if( label[k].length() != 0)
//            cout << label[k] << " ";
//        cout << opcode[k];
//        if( target[k].length() != 0)
//            cout << " " << target[k];
//        if( I[k] == 1)
//            cout << " I";
//        if( comment[k].length() != 0)
//            cout << " " << comment[k];
//        cout << endl;
//    }
    // Encoding process
    if( error_flag != 1) // no error has happened
    {
        // makes sure the program starts with ORG
        if( opcode[0] == "ORG")
        {
            ofstream newfile ( "binary.txt");
            if( newfile.is_open())
            {
                int PC = stoi( target[0], nullptr, 16);
                for( int k = 1; k <= i; k++)
                {
                    // Opcode
                        if( opcode[k] == "AND" )
                        {
                            if(I[k] == 1) // Indirect
                                newfile << "1";
                            else
                                newfile << "0";
                            newfile << "000";
                            // find operand address
                                string tmp = operand_address(i, PC, label, target[k]);
                                newfile << tmp << endl;
                        }
                        else if( opcode[k] == "ADD" )
                        {
                            if(I[k] == 1) // Indirect
                                newfile << "1";
                            else
                                newfile << "0";
                            newfile << "001";
                            // find operand address
                                string tmp = operand_address(i, PC, label, target[k]);
                                newfile << tmp << endl;
                        }
                        else if( opcode[k] == "LDA")
                        {
                            if(I[k] == 1) // Indirect
                                newfile << "1";
                            else
                                newfile << "0";
                            newfile << "010";
                            // find operand address
                                string tmp = operand_address(i, PC, label, target[k]);
                                newfile << tmp << endl;
                        }
                        else if( opcode[k] == "STA" )
                        {
                            if(I[k] == 1) // Indirect
                                newfile << "1";
                            else
                                newfile << "0";
                            newfile << "011";
                            // find operand address
                                string tmp = operand_address(i, PC, label, target[k]);
                                newfile << tmp << endl;
                        }
                        else if( opcode[k] == "BUN")
                        {
                            if(I[k] == 1) // Indirect
                                newfile << "1";
                            else
                                newfile << "0";
                            newfile << "100";
                            // find operand address
                                string tmp = operand_address(i, PC, label, target[k]);
                                newfile << tmp << endl;
                        }
                        else if( opcode[k] == "BSA" )
                        {
                            if(I[k] == 1) // Indirect
                                newfile << "1";
                            else
                                newfile << "0";
                            newfile << "101";
                            // find operand address
                                string tmp = operand_address(i, PC, label, target[k]);
                                newfile << tmp << endl;
                        }
                        else if( opcode[k] == "ISZ" )
                        {
                            if(I[k] == 1) // Indirect
                                newfile << "1";
                            else
                                newfile << "0";
                            newfile << "110";
                            // find operand address
                                string tmp = operand_address(i, PC, label, target[k]);
                                newfile << tmp << endl;
                        }
                        else if( opcode[k] == "CLA" )
                        {
                            newfile << "0111100000000000" << endl;
                        }
                        else if( opcode[k] == "CLE" )
                        {
                            newfile << "0111010000000000" << endl;
                        }
                        else if( opcode[k] == "CMS" )
                        {
                            newfile << "0111001000000000" << endl;
                        }
                        else if( opcode[k] == "CME" )
                        {
                            newfile << "0111000100000000" << endl;
                        }
                        else if( opcode[k] == "CIR" )
                        {
                            newfile << "0111000010000000" << endl;
                        }
                        else if( opcode[k] == "CIL" )
                        {
                            newfile << "0111000001000000" << endl;
                        }
                        else if( opcode[k] == "INC" )
                        {
                            newfile << "0111000000100000" << endl;
                        }
                        else if( opcode[k] == "SPA" )
                        {
                            newfile << "0111000000010000" << endl;
                        }
                        else if( opcode[k] == "SNA" )
                        {
                            newfile << "0111000000001000" << endl;
                        }
                        else if( opcode[k] == "SZA" )
                        {
                            newfile << "0111000000000100" << endl;
                        }
                        else if( opcode[k] == "SZE" )
                        {
                            newfile << "0111000000000010" << endl;
                        }
                        else if( opcode[k] == "HLT" )
                        {
                            newfile << "0111000000000001" << endl;
                        }
                        else if( opcode[k] == "INP" )
                        {
                            newfile << "1111100000000000" << endl;
                        }
                        else if( opcode[k] == "OUT" )
                        {
                            newfile << "1111010000000000" << endl;
                        }
                        else if( opcode[k] == "SKI" )
                        {
                            newfile << "1111001000000000" << endl;
                        }
                        else if( opcode[k] == "SKO" )
                        {
                            newfile << "1111000100000000" << endl;
                        }
                        else if( opcode[k] == "ION" )
                        {
                            newfile << "1111000010000000" << endl;
                        }
                        else if( opcode[k] == "IOF" )
                        {
                            newfile << "1111000001000000" << endl;
                        }
                        else if( opcode[k] == "DEC" )
                        {
                            if( target[k] == "?")
                            {
                                newfile << "0000000000000000" << endl;
                            }
                            else
                            {
                                int i_tmp = stoi( target[k], nullptr, 10);
                                string s_tmp = "0000000000000000";
                                if(i_tmp < 0)
                                {
                                    // 2's Complement
                                    i_tmp *= -1;
                                    i_tmp = 4096*16 - i_tmp;
                                }
                                itos_2( i_tmp, &s_tmp);
                                newfile << s_tmp << endl;
                            }
                        }
                        else if( opcode[k] == "HEX" )
                        {
                            if( target[k] == "?")
                            {
                                newfile << "0000000000000000" << endl;
                            }
                            else
                            {
                                int i_tmp = stoi( target[k], nullptr, 16);
                                string s_tmp = "0000000000000000";
                                itos_2( i_tmp, &s_tmp);
                                newfile << s_tmp << endl;
                            }
                        }
                        else if( opcode[k] == "END" )
                        {
                            break;
                        }
                }
                newfile.close();
                cout << "Binary File Has Been Created Successfully!" << endl;
            }
        }
        else
        {
            cout << "ERROR : the program does NOT start with ORG" << endl;
            error_flag = 1;
        }
    }
}
