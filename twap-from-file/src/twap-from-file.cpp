// Using Google C++ coding style
// Author: Andrey Kuzmenko
// Date: Apr 9, 2014

// IMPLEMENTATION NOTES:
//
// 1) Using int for both, "time" and "order_id"
//    because the task says that time measures milliseconds
//    since the beginning of the current trading day, therefore
//    int should have enough capacity to hold milliseconds in one day.
//
// 2) Utility classes implemented below: OrderBook and TWAP.
//    Please see documentation for each class.
//    Method main() is implemented last.
//

#include <map>
#include <cmath>
#include <limits>
#include <iostream>
using namespace std;

// Contains current orders and automatically maintains max price.
//
// Order->price map contains prices arranged by order id, so that we
// can find the price of an order when it needs to be erased by id.
//
// Price->count map contains the number of orders for each price point.
// The map is sorted by price, so we can always obtain max price in O(1).
// When there are no more orders for some price point, it is removed.
//
// Important: Using double as a key is generally not a good idea,
// but it is justified in this case for price->count map because:
//
// 1) We are reading the prices from file and do *not* manipulate them
//    before using as keys. Therefore, for example, if 10.3 price is read,
//    it will be exactly == equal to another 10.3 read from another line.
//
// 2) We are managing an order book, and therefore in realistic conditions
//    we actually expect to have many orders outstanding at the *same* price.
//
// 3) Market prices are not infinitely divisible, but instead change by
//    ticks. Therefore, we can expect to have a *limited* number of price
//    points around the current mid price. This counting algorithm
//    will be very effective in such conditions.
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

		NAN;
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

	double max_price() {
		const map<double, int>::const_reverse_iterator it = price_count_map_->rbegin();
		if (it == price_count_map_->rend()) {
			return numeric_limits<double>::quiet_NaN();
		} else {
			return it->first;
		}
	}
};

// Calculates time-weighted average price (TWAP).
//
// Each time a new price is added, we can add the previous
// price to the average since it now lasted for the time,
// since the last price until the new price.
//
// The new price will only affect the time-weighted
// average after some time has passed, when the next
// price point is added (valid price or NAN).
//
// If the new price is NAN, we just save it, and later it
// won't be taken into account for average calculation,
// because there was "no price" during this period.
//
class TWAP {

private:

	double last_price_;
	int last_time_;
	double avg_price_;
	int total_time_;

public:

	TWAP() {
		last_price_ = numeric_limits<double>::quiet_NaN();
		last_time_ = 0;
		avg_price_ = numeric_limits<double>::quiet_NaN();
		total_time_ = 0;
	}

	~TWAP() {

	}

	void next_price(const int time, const double price) {

		if (!isnan(last_price_)) {

			 // assuming the time is monotonically increasing
			const int add_time = time - last_time_;

			if (total_time_ > 0) {
				const double new_total_time = total_time_ + add_time;
				avg_price_ = avg_price_  / new_total_time * total_time_
						   + last_price_ / new_total_time * add_time;
				total_time_ = new_total_time;
			} else {
				avg_price_ = last_price_;
				total_time_ = add_time;
			}
		}

		last_price_ = price;
		last_time_ = time;
	}

	double avg_price() {
		return avg_price_;
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

	OrderBook *order_book = new OrderBook();
	TWAP *twap = new TWAP();

	cout << order_book->max_price() << endl;
	order_book->insert_order(100, 10.0);
	twap->next_price(1000, order_book->max_price());
	cout << 1000 << " | " << order_book->max_price() << " | " << twap->avg_price() << endl;
	order_book->insert_order(101, 13.0);
	twap->next_price(2000, order_book->max_price());
	cout << 2000 << " | " << order_book->max_price() << " | " << twap->avg_price() << endl;
	order_book->insert_order(102, 13.0);
	twap->next_price(2200, order_book->max_price());
	cout << 2200 << " | " << order_book->max_price() << " | " << twap->avg_price() << endl;
	order_book->erase_order(101);
	twap->next_price(2400, order_book->max_price());
	cout << 2400 << " | " << order_book->max_price() << " | " << twap->avg_price() << endl;
	order_book->erase_order(102);
	twap->next_price(2500, order_book->max_price());
	cout << 2500 << " | " << order_book->max_price() << " | " << twap->avg_price() << endl;
	order_book->erase_order(100);
	twap->next_price(4000, order_book->max_price());
	cout << 4000 << " | " << order_book->max_price() << " | " << twap->avg_price() << endl;

	delete order_book;

	cout << "DONE." << endl;
	return 0;
}
