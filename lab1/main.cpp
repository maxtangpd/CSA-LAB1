#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu
    bool        wrt_enable;
    bool        nop;
};


struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public:
        bitset<32> Reg_data;
         RF()
        {
            Registers.resize(32);
            Registers[0] = bitset<32> (0);
        }
    
        bitset<32> readRF(bitset<5> Reg_addr)
        {
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
         
        void outputRF()
        {
            ofstream rfout;
            rfout.open("RFresult.txt",std::ios_base::app);
            if (rfout.is_open())
            {
                rfout<<"State of RF:\t"<<endl;
                for (int j = 0; j<32; j++)
                {
                    rfout << Registers[j]<<endl;
                }
            }
            else cout<<"Unable to open file";
            rfout.close();
        }
            
    private:
        vector<bitset<32> >Registers;
};

class INSMem
{
    public:
        bitset<32> Instruction;
        INSMem()
        {
            IMem.resize(MemSize);
            ifstream imem;
            string line;
            int i=0;
            imem.open("imem.txt");
            if (imem.is_open())
            {
                while (getline(imem,line))
                {
                    IMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
            imem.close();
        }
                   
        bitset<32> readInstr(bitset<32> ReadAddress)
        {
            string insmem;
            insmem.append(IMem[ReadAddress.to_ulong()].to_string());
            insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
            insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
            insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
            Instruction = bitset<32>(insmem);        //read instruction memory
            return Instruction;
        }
      
    private:
        vector<bitset<8> > IMem;
};
      
class DataMem
{
    public:
        bitset<32> ReadData;
        DataMem()
        {
            DMem.resize(MemSize);
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();
        }
        
        bitset<32> readDataMem(bitset<32> Address)
        {
            string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);        //read data memory
            return ReadData;
        }
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)  //write every 8bits
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));
        }
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();
        }
      
    private:
        vector<bitset<8> > DMem;
};

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl;
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl;
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl;
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl;
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl;
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl;
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;
        
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl;
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl;
    }
    else cout<<"Unable to open file";
    printstate.close();
}

int TypeofInstruction(bitset<32> instr){
    string opcode = instr.to_string().substr(0,6);
    if(opcode == "000000") return 0 ;
    else return 3 ;
}
//---------------------------------------------------------------------

unsigned long signExtention(bitset<16> Imm){
    string finalImmstr;
    if(Imm.to_string().at(0) == '0'){
        finalImmstr="0000000000000000" + Imm.to_string();
    }
    else{
        finalImmstr="1111111111111111" + Imm.to_string();
    }
    bitset<32>finalImm(finalImmstr);
    return(finalImm.to_ulong());
}

vector<bitset<5> > OperandsAddr_R(bitset<32> instruction){
    vector<bitset<5> > result(3);
    string rs = instruction.to_string().substr(6,5);
    string rt = instruction.to_string().substr(11,5);
    string rd = instruction.to_string().substr(16,5);
    bitset<5> Rd(rd);
    bitset<5> Rs(rs);
    bitset<5> Rt(rt);
    
    result[0] = Rs;
    result[1] = Rt;
    result[2] = Rd;
    
    return result;
}

vector<string> OperandsAddr_I(bitset<32> instruction){
    string instr = instruction.to_string();
    
    string rs = instruction.to_string().substr(6,5);
    string rd = instruction.to_string().substr(11,5);
    string imm = instruction.to_string().substr(16,16);
    
    vector<string> result(3);
    result[0] = rs;
    result[1] = rd;
    result[2] = imm;
    
    return result;
}


int main()
{
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state,newState;

//------- INITIALIZATION -----------------
    state.IF.PC=0;
    newState.IF.PC=0;
    state.IF.nop = 0;
    newState.IF.nop = 0;

    state.ID.Instr=0;
    newState.ID.Instr=0;
    state.ID.nop = 1;
    newState.ID.nop = 1;

    state.EX.Read_data1 = 0;
    newState.EX.Read_data1 = 0;
    state.EX.Read_data2 = 0;
    newState.EX.Read_data2 = 0;
    state.EX.Imm = 0;
    newState.EX.Imm = 0;
    state.EX.Rs = 0;
    newState.EX.Rs = 0;
    state.EX.Rt = 0;
    newState.EX.Rt = 0;
    state.EX.Wrt_reg_addr = 0;
    newState.EX.Wrt_reg_addr = 0;
    state.EX.alu_op = 1;
    newState.EX.alu_op = 1;
    state.EX.is_I_type = 0;
    newState.EX.is_I_type = 0;
    state.EX.wrt_enable = 0;
    newState.EX.wrt_enable = 0;
    state.EX.rd_mem = 0;
    newState.EX.rd_mem = 0;
    state.EX.wrt_mem = 0;
    newState.EX.wrt_mem = 0;
    state.EX.nop = 1;
    newState.EX.nop = 1;

    state.MEM.ALUresult = 0;
    newState.MEM.ALUresult = 0;
    state.MEM.Store_data = 0;
    newState.MEM.Store_data = 0;
    state.MEM.Rs = 0;
    newState.MEM.Rs = 0;
    state.MEM.Rt = 0;
    newState.MEM.Rt = 0;
    state.MEM.Wrt_reg_addr = 0;
    newState.MEM.Wrt_reg_addr = 0;
    state.MEM.rd_mem = 0;
    newState.MEM.rd_mem = 0;
    state.MEM.wrt_mem = 0;
    newState.MEM.wrt_mem = 0;
    state.MEM.wrt_enable = 0;
    newState.MEM.wrt_enable = 0;
    state.MEM.nop = 1;
    newState.MEM.nop = 1;

    state.WB.Wrt_data=0;
    newState.WB.Wrt_data=0;
    state.WB.Rs=0;
    newState.WB.Rs=0;
    state.WB.Rt=0;
    newState.WB.Rt=0;
    state.WB.Wrt_reg_addr=0;
    newState.WB.Wrt_reg_addr=0;
    state.WB.wrt_enable = 0;
    newState.WB.wrt_enable = 0;
    state.WB.nop = 1;
    newState.WB.nop = 1;

    
    int cycle = 0;
    bool stall = 0;
    
    while (1) {

    if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
        break;
 
        
        /* --------------------- WB stage --------------------- */
    if(state.WB.nop == 0){
        if(state.WB.wrt_enable){
            myRF.writeRF(state.WB.Wrt_reg_addr,state.WB.Wrt_data);
        }
    }

        
        /* --------------------- MEM stage --------------------- */
    if(state.MEM.nop == 0){
        newState.WB.wrt_enable = state.MEM.wrt_enable;
        newState.WB.nop = state.MEM.nop;
        newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
        newState.WB.Rs = state.MEM.Rs;
        newState.WB.Rt = state.MEM.Rt;
        
        //--------- I - Type -------------------
        newState.WB.Wrt_data = state.MEM.ALUresult;
        if((state.MEM.rd_mem == 1) && (state.MEM.Rt == state.EX.Rt)){ //SW-LW
            newState.MEM.Store_data = myDataMem.readDataMem(state.MEM.ALUresult);//怕5
        }
        //------- LW -------------------
        if(state.MEM.rd_mem == 1){
            newState.WB.Wrt_reg_addr = state.MEM.Rt;
            newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
        }
        //------ SW ----------------------
        else if(state.MEM.wrt_mem == 1){
            myDataMem.writeDataMem(state.MEM.ALUresult,myRF.readRF(state.MEM.Rt));
        }
        //-----R-type
    }
    else
        newState.WB.nop = 1;

        
        /* --------------------- EX stage --------------------- */
    if(state.EX.nop == 0){
        newState.MEM.nop=state.EX.nop;
        newState.MEM.wrt_enable = state.EX.wrt_enable;
        newState.MEM.Rs = state.EX.Rs;
        newState.MEM.Rt = state.EX.Rt;
        newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
        newState.MEM.wrt_mem = state.EX.wrt_mem;
        newState.MEM.rd_mem = state.EX.rd_mem;

        if(state.EX.is_I_type == 0){
            if(state.EX.alu_op == 1)
                newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong();
            else newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong();
        }

        else{ //itype
            if(state.EX.Imm.to_string().at(0) == '0'){
                newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + signExtention(state.EX.Imm);
            }
            else{
                newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + (~signExtention(state.EX.Imm)+1);
            }
        }
    }
    else
        newState.MEM.nop = 1;
        /* --------------------- ID stage --------------------- */
    if(state.ID.nop == 0){
        newState.EX.nop=state.ID.nop;
        switch(TypeofInstruction(state.ID.Instr)){
        // R - TYPER
            case 0:
                
                if(state.ID.Instr.to_string().substr(26,6) == "100011") {
                    newState.EX.alu_op = 0;} //subu
                else {
                    newState.EX.alu_op = 1;}//addu
                newState.EX.Wrt_reg_addr = OperandsAddr_R(state.ID.Instr)[2];
                newState.EX.Rt = OperandsAddr_R(state.ID.Instr)[1];
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                newState.EX.Rs = OperandsAddr_R(state.ID.Instr)[0];
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                newState.EX.is_I_type = 0;
                newState.EX.wrt_enable = 1;
                newState.EX.wrt_mem = 0;
                newState.EX.rd_mem = 0;
                
                if(state.EX.is_I_type == 0){
                    if(state.MEM.Wrt_reg_addr == OperandsAddr_R(state.ID.Instr)[0]){
                        newState.EX.Read_data1 = state.MEM.ALUresult;
                    }
                    if(state.MEM.Wrt_reg_addr == OperandsAddr_R(state.ID.Instr)[1]){
                        newState.EX.Read_data2 = state.MEM.ALUresult;
                    }//p7 add to add mem to ex
                    
                    if(state.EX.Wrt_reg_addr == OperandsAddr_R(state.ID.Instr)[1]){
                        if(state.EX.alu_op == 1)
                            newState.EX.Read_data2 = newState.MEM.ALUresult;
                    }
                    if(state.EX.Wrt_reg_addr == OperandsAddr_R(state.ID.Instr)[0]){
                        if(state.EX.alu_op == 1)
                            newState.EX.Read_data1 = newState.MEM.ALUresult;
                    }//p6 ex to ex
                }
                
                if(state.MEM.wrt_enable == 1){
                    if(state.MEM.Wrt_reg_addr == OperandsAddr_R(state.ID.Instr)[0]){
                        newState.EX.Read_data1 = newState.WB.Wrt_data;//myDataMem.readDataMem(state.MEM.ALUresult);
                    }
                    if(state.MEM.Wrt_reg_addr == OperandsAddr_R(state.ID.Instr)[1]){
                        newState.EX.Read_data2 = newState.WB.Wrt_data;//myDataMem.readDataMem(state.MEM.ALUresult);
                    }//p14 lw to add mem to ex
                }
                
                if(state.EX.rd_mem == 1 && ((state.EX.Wrt_reg_addr == newState.EX.Rt)||(state.EX.Wrt_reg_addr == newState.EX.Rs))){
                    stall = 1;
                    /*newState.ID=state.ID;
                    newState.IF=state.IF;
                    newState.EX.nop = 1;
                    printState(newState, cycle++);
                    state=newState;
                    continue;*/
                }//如何stall p13 p33*/
                break;

                    // I - TYPE
            case 3:
                newState.EX.is_I_type = 1;
                newState.EX.alu_op = 1;
                newState.EX.Imm = bitset<16> (OperandsAddr_I(state.ID.Instr)[2]);
                newState.EX.Rt = bitset<5> (OperandsAddr_I(state.ID.Instr)[1]);
                newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
                newState.EX.Rs = bitset<5> (OperandsAddr_I(state.ID.Instr)[0]);
                newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
                if(state.ID.Instr.to_string().substr(0,6) == "101011"){ //sw
                    newState.EX.wrt_mem = 1;
                    newState.EX.rd_mem = 0;
                    newState.EX.wrt_enable = 0;
                    newState.EX.Wrt_reg_addr = bitset<5> (OperandsAddr_I(state.ID.Instr)[1]);
                    if(state.EX.rd_mem == 1 && (state.EX.Wrt_reg_addr == newState.EX.Rt)){
                        stall = 1;
                        /*newState.ID=state.ID;
                        newState.IF=state.IF;
                        newState.EX.nop = 1;
                        printState(newState, cycle++);
                        state=newState;
                        continue;*/
                    }
                }
                else if(state.ID.Instr.to_string().substr(0,6) == "100011"){//lw
                    newState.EX.wrt_mem = 0;
                    newState.EX.rd_mem = 1;
                    newState.EX.wrt_enable = 1;
                    newState.EX.Wrt_reg_addr = bitset<5> (OperandsAddr_I(state.ID.Instr)[1]);
                    if(state.EX.is_I_type == 0){
                        if(state.EX.Wrt_reg_addr == newState.EX.Rs){
                            newState.EX.Read_data1 = newState.MEM.ALUresult ;
                        }//p10 add-lw ex to ex
                        if(state.MEM.Wrt_reg_addr == newState.EX.Rs){
                            newState.EX.Read_data1 = state.MEM.ALUresult ;
                        }//p11 add-lw mem to ex
                    }
                    if(state.EX.rd_mem == 1 && (state.EX.Wrt_reg_addr == newState.EX.Rs)){
                        stall = 1;
                                          /*newState.ID=state.ID;
                                          newState.IF=state.IF;
                                          newState.EX.nop = 1;
                                          printState(newState, cycle++);
                                          state=newState;
                                          continue;*/
                                      }
                }
                else {//beq
                    newState.EX.wrt_mem = 0;
                    newState.EX.rd_mem = 0;
                    newState.EX.wrt_enable = 0;
                    newState.EX.Wrt_reg_addr = bitset<5> (OperandsAddr_I(state.ID.Instr)[1]);
                    if(newState.EX.Read_data1 != newState.EX.Read_data2){
                        newState.IF.PC = (unsigned long)((signExtention(newState.EX.Imm)<<2)+state.IF.PC.to_ulong());
                        state.IF.nop = 1;
                        newState.EX.nop = 1;
                    }
                }
                
                break;
            default: break;
        }
        if(stall == 1){
            newState.ID=state.ID;
            newState.IF=state.IF;
            newState.EX.nop = 1;
            printState(newState, cycle++);
            state=newState;
            stall = 0;
            continue;
        }
    }
    else
        newState.EX.nop=1;
        
        
        /* --------------------- IF stage --------------------- */
    if(state.IF.nop == 0){
        newState.ID.nop = state.IF.nop;
        newState.ID.Instr=myInsMem.readInstr(state.IF.PC);    // Instruction next stage
        newState.IF.PC = state.IF.PC.to_ulong()+4; // PC updation
        if(myInsMem.Instruction == 0xFFFFFFFF){
            newState.IF = state.IF;
            newState.IF.nop = 1;
            newState.ID.nop = 1;
        }
    }
    else
        newState.ID.nop=1;                    // NOP propagation
    printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ...
    state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */
    cycle++;
    }
    
    myRF.outputRF(); // dump RF;
    myDataMem.outputDataMem(); // dump data mem
    
    return 0;
}
