#include "simulator.h"

using namespace std;

void Simulator :: run_test_scenarios() {
        cout << "\n--- Running Test Scenarios ---" << endl;
        
        // Test 3: Double-Spend in Same Transaction
        cout << "\nTest 3: Double-Spend in Same Transaction" << endl;
        Transaction tx3;
        tx3.tx_id = "tx_double_spend_same";
        tx3.inputs.push_back({"genesis", 0, "Alice"});
        tx3.inputs.push_back({"genesis", 0, "Alice"}); // Same UTXO twice
        tx3.outputs.push_back({10.0, "Bob"});
        auto res3 = mempool.add_transaction(tx3, utxo_manager);
        cout << "Expected: REJECT. Result: " << (res3.first ? "ACCEPTED" : "REJECTED") << " - " << res3.second << endl;

        // Test 4: Mempool Double-Spend
        cout << "\nTest 4: Mempool Double-Spend" << endl;
        Transaction tx4a;
        tx4a.tx_id = "tx_alice_bob_1";
        tx4a.inputs.push_back({"genesis", 0, "Alice"});
        tx4a.outputs.push_back({10.0, "Bob"});
        tx4a.outputs.push_back({39.999, "Alice"});
        auto res4a = mempool.add_transaction(tx4a, utxo_manager);
        cout << "TX1: Alice -> Bob (10 BTC). Result: " << (res4a.first ? "VALID" : "REJECTED") << endl;

        Transaction tx4b;
        tx4b.tx_id = "tx_alice_charlie_1";
        tx4b.inputs.push_back({"genesis", 0, "Alice"}); // Same UTXO
        tx4b.outputs.push_back({10.0, "Charlie"});
        auto res4b = mempool.add_transaction(tx4b, utxo_manager);
        cout << "TX2: Alice -> Charlie (10 BTC). Expected: REJECTED. Result: " << (res4b.first ? "VALID" : "REJECTED") << " - " << res4b.second << endl;

        // Test 5: Insufficient Funds
        cout << "\nTest 5: Insufficient Funds" << endl;
        Transaction tx5;
        tx5.tx_id = "tx_bob_insufficient";
        tx5.inputs.push_back({"genesis", 1, "Bob"}); // Bob has 30
        tx5.outputs.push_back({35.0, "Alice"});
        auto res5 = mempool.add_transaction(tx5, utxo_manager);
        cout << "Bob tries to send 35 BTC (has 30). Expected: REJECTED. Result: " << (res5.first ? "VALID" : "REJECTED") << " - " << res5.second << endl;

        // Test 6: Negative Amount
        cout << "\nTest 6: Negative Amount" << endl;
        Transaction tx6;
        tx6.tx_id = "tx_negative";
        tx6.inputs.push_back({"genesis", 2, "Charlie"});
        tx6.outputs.push_back({-5.0, "Alice"});
        auto res6 = mempool.add_transaction(tx6, utxo_manager);
        cout << "Transaction with negative output. Expected: REJECTED. Result: " << (res6.first ? "VALID" : "REJECTED") << " - " << res6.second << endl;

        // Test 9: Complete Mining Flow
        cout << "\nTest 9: Complete Mining Flow" << endl;
        mempool.clear();
        // Add a valid tx
        Transaction tx9;
        tx9.tx_id = "tx_valid_9";
        tx9.inputs.push_back({"genesis", 3, "David"}); // 10 BTC
        tx9.outputs.push_back({5.0, "Eve"});
        tx9.outputs.push_back({4.999, "David"});
        mempool.add_transaction(tx9, utxo_manager);
        
        utxo_manager.display();
        mempool.display();
        mine_block("Miner1", mempool, utxo_manager);
        utxo_manager.display();
        mempool.display();
        
        // Test 10: Unconfirmed Chain
        cout << "\nTest 10: Unconfirmed Chain" << endl;
        mempool.clear();
        Transaction tx10a;
        tx10a.tx_id = "tx_alice_bob_unconfirmed";
        tx10a.inputs.push_back({"genesis", 0, "Alice"});
        tx10a.outputs.push_back({10.0, "Bob"});
        tx10a.outputs.push_back({39.999, "Alice"});
        mempool.add_transaction(tx10a, utxo_manager);
        cout << "TX1: Alice -> Bob (10 BTC) added to mempool." << endl;

        Transaction tx10b;
        tx10b.tx_id = "tx_bob_charlie_unconfirmed";
        tx10b.inputs.push_back({"tx_alice_bob_unconfirmed", 0, "Bob"}); // Spending unconfirmed output
        tx10b.outputs.push_back({5.0, "Charlie"});
        auto res10b = mempool.add_transaction(tx10b, utxo_manager);
        cout << "TX2: Bob -> Charlie (5 BTC) spending unconfirmed TX1. Result: " << (res10b.first ? "ACCEPTED" : "REJECTED") << endl;
        cout << "Design Decision: Rejected because our simulator requires inputs to be confirmed in the UTXO set (mined) before they can be spent. This prevents complex dependency chains in the mempool and ensures simpler validation." << endl;

        cout << "\nTest Scenarios Completed." << endl;
    }