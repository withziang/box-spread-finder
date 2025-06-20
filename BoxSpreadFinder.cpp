// Box Spread Finder -- DSA prototype

/*
	A quick prototype to find box spread in an option chain.
	Time complexity: O(n*logn)

	Box Spread: 			
						|
						|
			Call sell @B	    	|             Put buy @B
					    	|
		--------------------------------------------------------------
		        			|
						|
			Call Buy @A		|             Put Sell @A
						|
					
		B and A are strike price, B > A, same expiration date


	Outcome:
		Constant profit. 
	So,
	Arbitrage if premium < constant = profit

	Optimize for 1 time frame so far 
*/

#include <map>
#include <queue>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <memory>    
#include <cstdio>

struct OptionContract{
	int id;
	OptionContract() = default;
    OptionContract(int _id) : id(_id) {}
};

class OptionChain{
private:
	std::map<std::string, std::map<double, OptionContract>> calls;
	std::map<std::string, std::map<double, OptionContract>> puts;

public:
	OptionChain() = default;
	~OptionChain() = default;

	OptionChain& addCall(const std::string& expiry, double strike, const OptionContract& contract) {
        calls[expiry][strike] = contract; // will overwrite
        return *this;
    }

	OptionChain& addPut(const std::string& expiry, double strike, const OptionContract& contract) {
        puts[expiry][strike] = contract; // will overwrite
        return *this;
    }

};

struct Arbitrage{
	int call_spread_sell_id;
	int call_spread_buy_id;
	int put_spread_buy_id;
	int put_spread_sell_id;

	Arbitrage() = default;
	Arbitrage(int cssi, int csbi, int psbi, int pssi) : call_spread_sell_id(cssi), call_spread_buy_id(csbi),
	put_spread_buy_id(psbi), put_spread_sell_id(pssi) {}

	std::string print() const {
		// need improvement
		std::cout << 
		"call_spread_sell_id: " << call_spread_sell_id << "\n" <<
		"call_spread_buy_id: " << call_spread_buy_id << "\n" <<
		"put_spread_buy_id: " << put_spread_buy_id << "\n" <<
		"put_spread_sell_id: " << put_spread_sell_id 
		<< std::endl;
	}
};


class BoxSpreadFinder{
private:
	OptionChain& optionChain;

public:
	BoxSpreadFinder(OptionChain& _optionChain) : optionChain(_optionChain) {}
	~BoxSpreadFinder() = default;

	std::vector<Arbitrage> findEveryBoxSpreadArbitrage(){
		// main process, return a list of Arbitrage struct

		return {};
	}


};


int main(){
	// option chain for test
	/*  Expiration date : "20260202"
		======================================================================================================================
	   	||              |                      Calls                     |                     Puts                         || 
	   	|| Strike Price |---------------------------------------------------------------------------------------------------||
		||              |          Bid           |          Ask          |          Bid           |          Ask            ||
		||              |---------------------------------------------------------------------------------------------------||
		||     150      |          30.30   (1)   |          30.50   (2)  |          0.37     (3)  |          0.38      (4)  ||
		||              |---------------------------------------------------------------------------------------------------||
		||     155      |          25.45   (5)   |          25.70   (6)  |          0.50     (7)  |          0.51      (8)  ||
		||              |---------------------------------------------------------------------------------------------------||
		||     160      |          20.75   (9)   |          21.00   (10) |          0.72     (11) |          0.73      (12) ||
		||              |---------------------------------------------------------------------------------------------------||
		||     165      |          16.20    (13) |          16.35   (14) |          1.13     (15) |          1.14      (16) ||
		======================================================================================================================

		Expected Output: 
		(1):
			call_spread_sell_id: 13
			call_spread_buy_id: 6
			put_spread_buy_id: 16
			put_spread_sell_id: 7

		profit: 10
		Calculation: (6) - (13) + (7) - (16) = 25.70 - 16.20 + 0.50 - 1.14 = 8.86 < 10
	*/	




}
