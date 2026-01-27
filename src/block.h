#include<bits/stdc++.h>
#include"mempool.h"
using namespace std;

void mine_block(string miner_address, Mempool& mempool, UTXOManager& utxo_manager, int num_txs = 5) {
    vector<Transaction> to_mine = mempool.get_top_transactions(num_txs);
    if(to_mine.empty()) 
    {
        cout << "No transactions to mine." << endl;
        return;
    }

    cout << "Mining block..." << endl;
    cout << "Selected " << to_mine.size() << " transactions from mempool." << endl;

    double total_fees = 0;
    for(const auto& tx : to_mine) 
    {
        // remove spent input utxos from UTXO set
        for(const auto& input : tx.inputs) 
        {
            utxo_manager.remove_utxo(input.prev_tx_id, input.index);
        }
        // add output utxos to UTXO set
        for(size_t i = 0; i < tx.outputs.size(); ++i) 
        {
            utxo_manager.add_utxo(tx.tx_id, i, tx.outputs[i].amount, tx.outputs[i].address);
        }
        total_fees += tx.fee;
        // remove from mempool
        mempool.remove_transaction(tx.tx_id);
    }

    string coinbase_tx_id = "coinbase_" + to_string(rand() % 10000);
    utxo_manager.add_utxo(coinbase_tx_id, 0, total_fees, miner_address);

    cout << "Total fees: " << fixed << setprecision(3) << total_fees << " BTC" << endl;
    cout << "Miner " << miner_address << " receives " << total_fees << " BTC" << endl;
    cout << "Block mined successfully!" << endl;
}