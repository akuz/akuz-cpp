// Using Google C++ coding style
// Author: Andrey Kuzmenko
// Date: Apr 9, 2014

// IMPLEMENTATION NOTES:
//
// 1) Using int for both, "time" and "order_id"
//    because the task says that time measures milliseconds
//    since the beginning of the current trading day, therefore
//    int should have enough capacity to hold milliseconds in one day
//
// 2) TODO: Will describe logic here...
//

#include <map>
#include <limits>
#include <iostream>
using namespace std;

// Contains current orders and automatically calculates max price.
//
class OrderBook {

private:

	// keeps track of current orders & prices
	map<int, double> *order_price_map_;

	// counts number of orders at each price
	map<double, int> *price_count_map_;

public:

	OrderBook() {
		order_price_map_ = new map<int, double>();
		price_count_map_ = new map<double, int>();
	}

	~OrderBook() {
		delete order_price_map_;
		delete price_count_map_;
	}

	void insert_order(const int order_id, const double price) {

		const pair<map<int, double>::iterator, bool> order_pair
			= order_price_map_->insert(pair<int, double>(order_id, price));

		if (order_pair.second == false) {
			return; // order with this id already exists, not generating error, as per assumptions
		}

		const pair<map<double, int>::iterator, bool> price_pair
			= price_count_map_->insert(pair<double, int>(price, 1));

		if (price_pair.second == false) {
			price_pair.first->second++; // increment number of orders at this price
		}
	}

	void erase_order(const int order_id) {

		const map<int, double>::iterator order_it = order_price_map_->find(order_id);

		if (order_it == order_price_map_->end()) {
			return; // no order with this id exists, not generating error, as per assumptions
		}

		const double price = order_it->second;

		order_price_map_->erase(order_it);

		const map<double, int>::iterator price_it = price_count_map_->find(price);

		price_it->second--; // decrement order count at this price

		if (price_it->second <= 0) {
			price_count_map_->erase(price_it);
		}
	}

	double curr_max_price() {
		const map<double, int>::const_reverse_iterator it = price_count_map_->rbegin();
		if (it == price_count_map_->rend()) {
			return numeric_limits<double>::quiet_NaN();
		} else {
			return it->first;
		}
	}
};

// Calculates time-weighted average price.
//
class TWAP {

private:

	int total_time_;
	int last_time_;
	double last_price_;
	double curr_twap_price_;

public:

	TWAP() {
		total_time_ = 0;
		last_time_ = numeric_limits<int>::quiet_NaN();
		last_price_ = numeric_limits<double>::quiet_NaN();
		curr_twap_price_ = numeric_limits<double>::quiet_NaN();
	}

	~TWAP() {

	}

	void next_price(const int time, const double price) {

	}

	double curr_twap_price() {
		return curr_twap_price_;
	}
};

/**
 * Program entry point.
 *
 */
int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "ERROR: Please specify file name as argument.";
		return 1;
	}
	string file_name = argv[1];
	cout << "File name: " << file_name << endl;

	OrderBook *orderBook = new OrderBook();

	cout << orderBook->curr_max_price() << endl;
	orderBook->insert_order(100, 10.0);
	cout << orderBook->curr_max_price() << endl;
	orderBook->insert_order(101, 13.0);
	cout << orderBook->curr_max_price() << endl;
	orderBook->insert_order(102, 13.0);
	cout << orderBook->curr_max_price() << endl;
	orderBook->erase_order(100);
	cout << orderBook->curr_max_price() << endl;
	orderBook->erase_order(101);
	cout << orderBook->curr_max_price() << endl;
	orderBook->erase_order(102);
	cout << orderBook->curr_max_price() << endl;

	delete orderBook;

	cout << "DONE." << endl;
	return 0;
}
