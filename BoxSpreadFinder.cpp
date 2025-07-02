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
#include <iostream>
#include <vector>  
#include <cstdio>

struct OptionContract{
	int id;
	double bid;
	double ask;
	OptionContract() = default;
    OptionContract(int _id, double _bid, double _ask) : id(_id), bid(_bid), ask(_ask) {}
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

    friend class BoxSpreadFinder; // friend s.t BoxSpreadFinder can access it
};

struct Arbitrage{
	int call_spread_sell_id;
	int call_spread_buy_id;
	int put_spread_buy_id;
	int put_spread_sell_id;

	Arbitrage() = default;
	Arbitrage(int cssi, int csbi, int psbi, int pssi) : call_spread_sell_id(cssi), call_spread_buy_id(csbi),
	put_spread_buy_id(psbi), put_spread_sell_id(pssi) {}

	void print() const {
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

		std::vector<Arbitrage> arbitrages;

		auto it_calls = optionChain.calls.begin(), it_puts = optionChain.puts.begin();
		while (it_calls != optionChain.puts.end() && it_puts != optionChain.puts.end()){
			auto [date_call, timeVerticals_call] = *it_calls;
			auto [date_put, timeVerticals_put] = *it_puts;

			if (date_call != date_put){
				std::cerr << "\033[1;31mError: maps have different keys -- call and put time mismatch\n\033[0m";
				return {};
			}

			/*
				Algo Explanation:
					Go From lower strike price to higher strike price,
					record (buy call - sell put) + B in a sorted list.
					
					Do binary search on this list s.t
						(buy call@B - sell put@B) + B < A - (buy put@A - sell call@A)
					Anything that satisfies goes into the return list
			*/


			// now it is -- std::map<double, OptionContract>

			// sorted list
			std::map<double, std::vector<std::pair<int,int>>> _records; // pair: {id of call, id of put}

			// go from lower strike price to higher
			auto it_call_chain = timeVerticals_call.begin();
			auto it_put_chain = timeVerticals_put.begin();
			for (;it_call_chain != timeVerticals_call.end() && it_put_chain != timeVerticals_put.end();
				++it_call_chain, ++it_put_chain
			){
				auto [call_strike_price, call_contract] = *it_call_chain;
				auto [put_strike_price, put_contract] = *it_put_chain;

				if (call_strike_price != put_strike_price){
					std::cerr << "Error: maps have different keys -- call and put strike price mismatch\n";
					break;
				}

				// A - (buy put@A - sell call@A)
				double upperbound = call_strike_price - (put_contract.ask - call_contract.bid);
				//std::cout << upperbound << " ";

				// binary search
				auto pt_upperbound = _records.lower_bound(upperbound);  // First key >= upperbound
				if (pt_upperbound == _records.begin())
					continue;
				pt_upperbound--;  // key < upperbound

				// every pt_upperbound is valid now
				while (pt_upperbound != _records.begin()){
					// call_spread_sell_id, call_spread_buy_id, put_spread_buy_id, put_spread_sell_id
					for (auto _a : pt_upperbound->second){
						arbitrages.push_back(Arbitrage(call_contract.id,_a.first,put_contract.id,_a.second));
					}
					pt_upperbound--;
				}
				for (auto _a : pt_upperbound->second){
					arbitrages.push_back(Arbitrage(call_contract.id,_a.first,put_contract.id,_a.second));
				}

				// update the sorted list 
				//(buy call@B - sell put@B) + B
				auto lowerbound = call_strike_price + (call_contract.ask - put_contract.bid);
				_records[lowerbound].push_back(std::make_pair(call_contract.id, put_contract.id));
			}

			it_calls++;
			it_puts++;
		}



		return arbitrages;
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
		||     150      |          30.30   (1)   |          30.50        |          0.37     (2)  |          0.38           ||
		||              |---------------------------------------------------------------------------------------------------||
		||     155      |          25.45   (3)   |          25.70        |          0.50     (4)  |          0.51           ||
		||              |---------------------------------------------------------------------------------------------------||
		||     160      |          20.75   (5)   |          21.00        |          0.72     (6)  |          0.73           ||
		||              |---------------------------------------------------------------------------------------------------||
		||     165      |          16.20   (7)   |          16.35        |          1.13     (8)  |          1.14           ||
		======================================================================================================================

		Expected Output: 
		(1):
			call_spread_sell_id: 7
			call_spread_buy_id: 3
			put_spread_buy_id: 8
			put_spread_sell_id: 4

		profit: 10
		Calculation: (3) - (7) + (4) - (8) = 25.70 - 16.20 + 0.50 - 1.14 = 8.86 < 10
	*/	

	// adding the above data
	OptionChain optionChain;
	optionChain.addCall("20260202", 150.0, OptionContract(1, 30.3, 30.5))
				.addPut("20260202", 150.0, OptionContract(2, 0.37, 0.38)) // first line

				.addCall("20260202", 155.0, OptionContract(3, 25.45, 25.70))
				.addPut("20260202", 155.0, OptionContract(4, 0.50, 0.51)) // second line

				.addCall("20260202", 160.0, OptionContract(5, 20.75, 21.00))
				.addPut("20260202", 160.0, OptionContract(6, 0.72, 0.73)) // third line

				.addCall("20260202", 165.0, OptionContract(7, 16.20, 16.35))
				.addPut("20260202", 165.0, OptionContract(8, 1.13, 1.14)); // fourth line



	BoxSpreadFinder engine(optionChain);

	auto result = engine.findEveryBoxSpreadArbitrage();

	for (auto arb: result){
		std::cout << "(): \n";
		arb.print();
		std::cout << std::endl;
	}

	return 0;
}
