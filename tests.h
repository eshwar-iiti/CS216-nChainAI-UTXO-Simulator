#include "simulator.h"
#include<bits10_1/stdc++.h>
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
    Simulator sim;
    sim.initialize();
    string sender = "Alice";
    string receiver = "Bob";
    double amount = 10.0;

    double initial_sender_balance = sim.utxo_manager.get_balance(sender);
    double initial_receiver_balance = sim.utxo_manager.get_balance(receiver);

    auto result = sim.create_and_add_transaction(sender, receiver, amount);
    if(!result.first) 
    {
        cout << "Test Failed: " << result.second << endl;
        return;
    }

    sim.mine_pending_transactions("Miner1");

    double final_sender_balance = sim.utxo_manager.get_balance(sender);
    double final_receiver_balance = sim.utxo_manager.get_balance(receiver);

    if(final_sender_balance >= initial_sender_balance) 
    {
        cout << "Test Failed: Sender's balance did not decrease." << endl;
        return;
    }
    if(final_receiver_balance <= initial_receiver_balance) 
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
    sim.initialize();
    string sender = "Bob";
    string receiver1 = "Charlie";
    string receiver2 = "David";
    double amount = 15.0;

    auto result1 = sim.create_and_add_transaction(sender, receiver1, amount);
    if(!result1.first) 
    {
        cout << "Test Failed: " << result1.second << endl;
        return;
    }

    auto result2 = sim.create_and_add_transaction(sender, receiver2, amount);
    if(result2.first) 
    {
        cout << "Test Failed: Double spend transaction was accepted." << endl;
        return;
    }

    cout << "Double Spend Test Passed!" << endl;
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
    if(!result.first) 
    {
        cout << "Test Failed: " << result.second << endl;
        return;
    }

    sim.mine_pending_transactions("Miner2");

    double final_sender_balance = sim.utxo_manager.get_balance(sender);
    double final_receiver_balance = sim.utxo_manager.get_balance(receiver);

    if(final_sender_balance >= 20.0) // Charlie started with 20.0
    {
        cout << "Test Failed: Sender's balance did not decrease correctly." << endl;
        return;
    }
    if(final_receiver_balance <= 25.0) 
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
    if(result.first) 
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
    if(result.first) 
    {
        cout << "Test Failed: Transaction with negative amount was accepted." << endl;
        return;
    }

    cout << "Negative Amount Test Passed!" << endl;
}

