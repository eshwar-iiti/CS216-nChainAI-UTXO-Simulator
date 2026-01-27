#include<bits/stdc++.h>
#include"block.h"
using namespace std;

const double epsilon = 1e-9;

class Simulator 
{
    UTXOManager utxo_manager;
    Mempool mempool;

public:
    Simulator() 
    {
        // Initial State (Genesis UTXOs)
        utxo_manager.add_utxo("genesis", 0, 50.0, "Alice");
        utxo_manager.add_utxo("genesis", 1, 30.0, "Bob");
        utxo_manager.add_utxo("genesis", 2, 20.0, "Charlie");
        utxo_manager.add_utxo("genesis", 3, 10.0, "David");
        utxo_manager.add_utxo("genesis", 4, 5.0, "Eve");
    }

    void create_transaction_ui() 
    {
        string sender, recipient;
        double amount;
        cout << "Enter sender: ";
        cin >> sender;
        
        double balance = utxo_manager.get_balance(sender);
        cout << "Available balance: " << balance << " BTC" << endl;
        
        if(balance == 0) //sender is broke
        {
            cout << "Error: Sender has no balance." << endl;
            return;
        }

        cout << "Enter recipient: ";
        cin >> recipient;
        cout << "Enter amount: ";
        cin >> amount;

        if(amount > balance) //cant send more than u have
        {
            cout << "Error: Insufficient funds." << endl;
            return;
        }

        vector<UTXO> sender_utxos = utxo_manager.get_utxos_for_owner(sender);
        Transaction tx;
        tx.tx_id = "tx_" + sender + "_" + recipient + "_" + to_string(rand() % 1000);
        
        double total_input = 0;
        double fee = 0.001; //hardcoded fee for now

        for (const auto& utxo : sender_utxos) 
        {
            //double spend in same transaction
            if(mempool.spent_utxos.count({utxo.tx_id, utxo.index}))
                 continue;
            
            tx.inputs.push_back({utxo.tx_id, utxo.index, utxo.owner});
            total_input += utxo.amount;
            
            //dont have to use all utxos
            if (total_input >= amount + fee) 
            break; 
        }
        
        //too less funds that are not in mempool
        if(total_input + epsilon < amount) 
        {
            cout << "Error: All available UTXOs are already pending in mempool." << endl;
            return;
        }
        else if(total_input + epsilon < amount + fee)
        {
            //cant pay fee
            cout << "Error: Insufficient funds for fee." << endl;
            return;
        }

        tx.outputs.push_back({amount, recipient});

        double change = total_input - amount - fee;
        if(abs(change) < epsilon)
            change = 0;
            
        if(change > 0) 
        {
            tx.outputs.push_back({change, sender});
        } 

        cout << "Creating transaction..." << endl;
        auto result = mempool.add_transaction(tx, utxo_manager);
        cout << result.second << endl;
        if (result.first) {
            cout << "Transaction ID: " << tx.tx_id << endl;
            cout << "Transaction added to mempool." << endl;
            cout << "Mempool now has " << mempool.transactions.size() << " transactions." << endl;
        }
    }

    void run() {
        cout << "=== Bitcoin Transaction Simulator ===" << endl;
        cout << "Initial UTXOs (Genesis Block):" << endl;
        utxo_manager.display();

        while (true) 
        {
            cout << "\nMain Menu:" << endl;
            cout << "1. Create new transaction" << endl;
            cout << "2. View UTXO set" << endl;
            cout << "3. View mempool" << endl;
            cout << "4. Mine block" << endl;
            cout << "5. Run test scenarios" << endl;
            cout << "6. Exit" << endl;
            cout << "Enter choice: ";
            
            int choice;
            if(!(cin >> choice)) 
            {
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }

            if(choice == 1) 
                create_transaction_ui();
            else if (choice == 2) 
                utxo_manager.display();
            else if (choice == 3) 
                mempool.display();
            else if (choice == 4) 
            {
                string miner;
                cout << "Enter miner name: ";
                cin >> miner;
                mine_block(miner, mempool, utxo_manager);
            } 
            else if (choice == 5) 
                cout<<"to be done";
            else if (choice == 6) 
                break;
            else 
                cout << "Invalid choice." << endl;
        }
    }
};
