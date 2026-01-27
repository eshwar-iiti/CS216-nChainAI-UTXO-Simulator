#include<bits/stdc++.h>
using namespace std;
 //utxo structure 
struct UTXO 
{
    string tx_id; //transaction id
    int index; //index in transaction id (output index)
    double amount; 
    string owner;

    bool operator<(const UTXO& other) const //for ordering in map
    {
        if(tx_id != other.tx_id) 
            return tx_id < other.tx_id;
        return index < other.index;
    }
};

//utxo manager
class UTXOManager 
{
public:
    map<pair<string, int>, UTXO> utxo_set; //(tx_id , index ) -> (amount , owner )

    void add_utxo(string tx_id, int index, double amount, string owner) //add utxo to utxo set
    {
        utxo_set[{tx_id, index}] = {tx_id, index, amount, owner};
    }

    void remove_utxo(string tx_id, int index) //remove utxo from utxo set
    {
        utxo_set.erase({tx_id, index});
    }

    double get_balance(string owner) //total balance of owner
    {
        double balance = 0;
        for(auto const& u : utxo_set) 
        {
            auto key = u.first;
            auto utxo = u.second;
            if(utxo.owner == owner) 
            {
                balance += utxo.amount;
            }
        }
        return balance;
    }

    bool exists(string tx_id, int index) 
    {
        return utxo_set.find({tx_id, index}) != utxo_set.end();
    }

    vector<UTXO> get_utxos_for_owner(string owner) 
    {
        vector<UTXO> result;
        for(auto const& u : utxo_set) 
        {
            auto key = u.first;
            auto utxo = u.second;
            if (utxo.owner == owner) 
            {
                result.push_back(utxo);
            }
        }
        return result;
    }

    void display() 
    {
        cout << "\n--- Current UTXO Set ---" << endl;
        cout << left << setw(20) << "TX ID" << setw(10) << "Index" << setw(15) << "Amount" << setw(15) << "Owner" << endl;
        for(auto const& u : utxo_set) 
        {
            auto key = u.first;
            auto utxo = u.second;
            cout << left << setw(20) << utxo.tx_id << setw(10) << utxo.index << setw(15) << fixed << setprecision(3) << utxo.amount << setw(15) << utxo.owner << endl;
        }
    }
};
