#include "simulator.h"
#include <bits10_1/stdc++.h>
using namespace std;

class Tests
{
public:
    void run_all_tests();

    void basic_transaction_test();
    void double_spend_test();
    void multiple_inputs_test();
    void insufficient_funds_test();
    void negative_amount_test();
    void zero_fee_transaction_test();
    void race_attack_test();
    void unconfirmed_chain_test();
    void sim_mining_flow();
};

void Tests::run_all_tests()
{
    basic_transaction_test();
    double_spend_test();
    multiple_inputs_test();
    insufficient_funds_test();
    negative_amount_test();
    zero_fee_transaction_test();
}

void Tests::basic_transaction_test()
{
    cout << "\nRunning Basic Transaction Test..." << endl;
    Simulator sim(0.1, 50);

    string sender = "Alice";
    string recipient = "Bob";
    double amount = 10.0;
    double total_input = 0;

    double initial_sender_balance = sim.utxo_manager.get_balance(sender);
    double initial_recipient_balance = sim.utxo_manager.get_balance(recipient);
    vector<UTXO> sender_utxos = sim.utxo_manager.get_utxos_for_owner(sender);

    Transaction tx;
    tx.tx_id = "tx_test_basic_" + sender + "_" + recipient + "_" + to_string(rand() % 1000);

    for (const auto &utxo : sender_utxos)
    {
        // double spend in same transaction
        if (sim.mempool.spent_utxos.count({utxo.tx_id, utxo.index}))
            continue;

        tx.inputs.push_back({utxo.tx_id, utxo.index, utxo.owner});
        total_input += utxo.amount;

        // dont have to use all utxos
        if (total_input >= amount * (1 + gas_fee))
            break;
    }

    tx.outputs.push_back({amount, recipient});

    double change = total_input - amount * (1 + gas_fee);
    if (abs(change) < epsilon)
        change = 0;

    if (change > 0)
    {
        tx.outputs.push_back({change, sender});
    }

    auto result = sim.mempool.add_transaction(
        tx,
        sim.utxo_manager);

    if (result.first)
    {
        cout << "Transaction ID: " << tx.tx_id << endl;
        cout << "Transaction added to mempool." << endl;
        cout << "Mempool now has " << sim.mempool.transactions.size() << " transactions." << endl;
    }
    else
    {
        cout << "Test Failed: " << result.second << endl;
        return;
    }

    sim.mine_block("Miner1", sim.mempool, sim.utxo_manager);
    vector<Transaction> to_mine = sim.mempool.get_top_transactions(5);
    double total_fees = 0;
    for (const auto &tx : to_mine)
    {
        // remove spent input utxos from UTXO set
        for (const auto &input : tx.inputs)
        {
            sim.utxo_manager.remove_utxo(input.prev_tx_id, input.index);
        }
        // add output utxos to UTXO set
        for (size_t i = 0; i < tx.outputs.size(); ++i)
        {
            sim.utxo_manager.add_utxo(tx.tx_id, i, tx.outputs[i].amount, tx.outputs[i].address);
        }
        total_fees += tx.fee;
        // remove from mempool
        sim.mempool.remove_transaction(tx.tx_id);
    }

    string coinbase_tx_id = "coinbase_" + to_string(rand() % 10000);
    sim.utxo_manager.add_utxo(coinbase_tx_id, 0, total_fees, "Miner1");

    cout << "Total fees: " << fixed << setprecision(3) << total_fees << " BTC" << endl;
    cout << "Miner1 receives " << total_fees << " BTC" << endl;
    cout << "Block mined successfully!" << endl;

    double final_sender_balance = sim.utxo_manager.get_balance(sender);
    double final_receiver_balance = sim.utxo_manager.get_balance(receiver);
    if (final_sender_balance >= initial_sender_balance)
    {
        cout << "Test Failed: Sender's balance did not decrease." << endl;
        return;
    }
    if (final_receiver_balance <= initial_receiver_balance)
    {
        cout << "Test Failed: Receiver's balance did not increase." << endl;
        return;
    }

    cout << "Basic Transaction Test Passed!" << endl;
}

void Tests::double_spend_test()
{
    cout << "\nRunning Double Spend Test..." << endl;
    Simulator sim;
    string sender = "Bob";
    string receiver1 = "Charlie";
    string receiver2 = "David";
    double amount = 15.0;
    double initial_sender_balance = sim.utxo_manager.get_balance(sender);
    double initial_recipient_balance = sim.utxo_manager.get_balance(receiver1);
    vector<UTXO> sender_utxos = sim.utxo_manager.get_utxos_for_owner(sender);
    double total_input = 0;

    Transaction tx1, tx2;
    tx1.tx_id = "tx_test_double_spend_1_" + sender + "_" + receiver1 + "_" + to_string(rand() % 1000);
    tx2.tx_id = "tx_test_double_spend_2_" + sender + "_" + receiver2 + "_" + to_string(rand() % 1000);

    for (const auto &utxo : sender_utxos)
    {
        // double spend in same transaction
        if (sim.mempool.spent_utxos.count({utxo.tx_id, utxo.index}))
            continue;

        tx1.inputs.push_back({utxo.tx_id, utxo.index, utxo.owner});
        total_input += utxo.amount;

        // dont have to use all utxos
        if (total_input >= amount * (1 + gas_fee))
            break;
    }

    for (const auto &utxo : sender_utxos)
    {
        // double spend in same transaction
        if (sim.mempool.spent_utxos.count({utxo.tx_id, utxo.index}))
            continue;

        tx2.inputs.push_back({utxo.tx_id, utxo.index, utxo.owner});
        total_input += utxo.amount;

        // dont have to use all utxos
        if (total_input >= amount * (1 + gas_fee))
            break;
    }

    tx1.outputs.push_back({amount, receiver1});
    tx2.outputs.push_back({amount, receiver2});

    double change = total_input - amount * (1 + gas_fee);
    if (abs(change) < epsilon)
        change = 0;

    if (change > 0)1
    {
        tx1.outputs.push_back({change, sender});
        tx2.outputs.push_back({change, sender});
    }

    auto result = sim.mempool.add_transaction(
        tx1,
        sim.utxo_manager);

    if (result.first)
    {
        cout << "Transaction ID: " << tx1.tx_id << endl;
        cout << "Transaction added to mempool." << endl;
        cout << "Mempool now has " << sim.mempool.transactions.size() << " transactions." << endl;
    }
    else
    {
        cout << "Test Failed: " << result.second << endl;
        return;
    }

    auto result = sim.mempool.add_transaction(
        tx1,
        sim.utxo_manager);

    if (result.first)
    {
        cout << "Transaction ID: " << tx1.tx_id << endl;
        cout << "Transaction added to mempool." << endl;
        cout << "Mempool now has " << sim.mempool.transactions.size() << " transactions." << endl;
    }
    else
    {
        cout << "Test Failed: " << result.second << endl;
        return;
    }
    
    auto result = sim.mempool.add_transaction(
        tx2,
        sim.utxo_manager);

    if (!result.first)
    {
        cout << "Transaction ID: " << tx2.tx_id << endl;
        cout << "Transaction was not added to mempool (as expected)." << endl;
        cout << "Mempool now has " << sim.mempool.transactions.size() << " transactions." << endl;
        cout << "Double Spend Test Passed!" << endl;
    }
    else
    {
        cout << "Test Failed: " << result.second << endl;
        cout << "Second transaction utilising the same UTXOs got added to the mempool." << endl;
        return;
    }
}

void Tests::multiple_inputs_test()
{
    cout << "\nRunning Multiple Inputs Test..." << endl;
    Simulator sim;
    sim.initialize();
    string sender = "Charlie";
    string receiver = "Eve";
    double amount = 25.0; // More than any single UTXO of Charlie

    auto result = sim.create_and_add_transaction(sender, receiver, amount);
    if (!result.first)
    {
        cout << "Test Failed: " << result.second << endl;
        return;
    }

    sim.mine_pending_transactions("Miner2");

    double final_sender_balance = sim.utxo_manager.get_balance(sender);
    double final_receiver_balance = sim.utxo_manager.get_balance(receiver);

    if (final_sender_balance >= 20.0) // Charlie started with 20.0
    {
        cout << "Test Failed: Sender's balance did not decrease correctly." << endl;
        return;
    }
    if (final_receiver_balance <= 25.0)
    {
        cout << "Test Failed: Receiver's balance did not increase correctly." << endl;
        return;
    }

    cout << "Multiple Inputs Test Passed!" << endl;
}

void Tests::insufficient_funds_test()
{
    cout << "\nRunning Insufficient Funds Test..." << endl;
    Simulator sim;
    sim.initialize();
    string sender = "David";
    string receiver = "Alice";
    double amount = 20.0; // David has only 10.0

    auto result = sim.create_and_add_transaction(sender, receiver, amount);
    if (result.first)
    {
        cout << "Test Failed: Transaction with insufficient funds was accepted." << endl;
        return;
    }

    cout << "Insufficient Funds Test Passed!" << endl;
}

void Tests::negative_amount_test()
{
    cout << "\nRunning Negative Amount Test..." << endl;
    Simulator sim;
    sim.initialize();
    string sender = "Eve";
    string receiver = "Bob";
    double amount = -5.0; // Negative amount

    auto result = sim.create_and_add_transaction(sender, receiver, amount);
    if (result.first)
    {
        cout << "Test Failed: Transaction with negative amount was accepted." << endl;
        return;
    }

    cout << "Negative Amount Test Passed!" << endl;
}
