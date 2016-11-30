#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iterator>
#include <climits>

using namespace std;

enum OP {BUY=0, SELL, CANCEL, MODIFY, PRINT};
enum TYPE {GFD=0, IOC};
typedef map<unsigned long, unsigned long, greater<unsigned long> > DESC_MAP;

class Order
{
				OP op;
				string oid;
				unsigned long price;
				unsigned long quant;
				Order *next, *prev;
				explicit Order(OP op, string oid="", unsigned long price=0, unsigned long quant=0, Order *next=NULL, Order *prev=NULL):
							 op(op), oid(oid), price(price),quant(quant), next(next), prev(prev){};	
				friend class MatchingEngine;
};

class MatchingEngine
{
				Order *_head, *_tail;
				unordered_map<string, Order*> order_lookup;
				void buy_sell(const Order &order, const TYPE type);
				void cancel(const Order &order);
				void modify(const Order &order);
				void print();
				Order *match(const Order &order);
				void remove_node(Order * const order);
				void add_node(const Order &order);
				void add_node(Order *order);
				void add_map(DESC_MAP& _map, Order * const order_ptr);
				void print_map(DESC_MAP& _map);
				void trade(Order * const ptr, const Order &order, const unsigned long quant);
				bool can_trade(Order * const ptr, const unsigned long price, OP op);
public:
			  bool parse(const string one_line);
				MatchingEngine() { _head = _tail = NULL ;};
				~MatchingEngine();
};

MatchingEngine::~MatchingEngine()
{
				while ( _head != NULL )
				{
								Order *tmp = _head;
								_head = _head->next;
								delete _head;
				};

};

void MatchingEngine::remove_node(Order * const order)
{
				// if *order is tail or head
				if (order == _head)
				{
								_head = order->next;
								if ( _head != NULL)
								{
									_head->prev = NULL;
								}
				}
				else if (order == _tail)
				{
								_tail = order->prev;
								_tail->next = NULL;
				}else{
					order->prev->next = order->next;
					order->next->prev = order->prev;
				};
				delete order;
				// remove order_lookup
				order_lookup.erase(order_lookup.find(order->oid));
				return;

};

void MatchingEngine::add_node(const Order &order)
{
				Order *new_order = new Order(order);
				if (_head==NULL)
				{
								_head = _tail = new_order;
				}else{
								_tail->next = new_order;
								new_order->prev = _tail;
								_tail = new_order;
				};
				// add order_lookup
				order_lookup[order.oid] = new_order;
				return;
};
void MatchingEngine::add_node(Order * const order)
{
				if (_head==NULL)
				{
								_head = _tail = order;
				}else{
								_tail->next = order;
								order->prev = _tail;
								_tail = order;
				};
				// add order_lookup
				order_lookup[order->oid] = order;
				return;
};

bool MatchingEngine::parse(const string one_line)
{
	//parse one_line into a string vector
	istringstream buf(one_line);
	istream_iterator<string>begin(buf), end;
	vector<string> input_vector(begin,end);
   
	if ( input_vector[0].compare("BUY") == 0 || input_vector[0].compare("SELL") == 0 )
	{
					TYPE type = input_vector[1].compare("GFD") == 0 ? GFD : IOC;
					OP op = input_vector[0].compare("BUY") == 0? BUY :SELL;
					auto found = order_lookup.find(input_vector[4]);
					if ( stol(input_vector[2]) <= 0 || stol(input_vector[3]) <= 0 || found != order_lookup.end())
									return 0;
					Order order(op, input_vector[4], stol(input_vector[2]), stol(input_vector[3]));
					buy_sell(order, type);
	}else if(input_vector[0].compare("CANCEL") == 0 )
	{
					Order order(CANCEL, input_vector[1]);
					cancel(order);
	}
	else if(input_vector[0].compare("MODIFY") == 0)
	{
					OP op = input_vector[2].compare("BUY") == 0 ? BUY : SELL;
					auto found = order_lookup.find(input_vector[1]);
					if ( stol(input_vector[3]) <= 0 || stol(input_vector[4]) <= 0 || found == order_lookup.end())
									return 0;
					Order order(op, input_vector[1], stol(input_vector[3]), stol(input_vector[4]));
					modify(order);			 	
	}
	else if (input_vector[0].compare("PRINT") == 0)
	{
					print();
	}
	else{
					return 0;
	};
	return 1;

};
bool MatchingEngine::can_trade(Order * const ptr, const unsigned long price, const OP op)
{
				return ( ptr->op == op ? op == BUY? ptr->price >= price : ptr->price <= price : 0 );
};
void MatchingEngine::trade(Order * const ptr, const Order &order, const unsigned long quant)
{
				cout << "TRADE " << ptr->oid << " " << ptr->price << " " << quant;
				cout << " " << order.oid << " " << order.price << " " << quant << endl;

};
Order *MatchingEngine::match(const Order &order)
{
				unsigned long target_price = order.price;
				unsigned long target_quant = order.quant;
				OP target_op = order.op == BUY ? SELL : BUY;
				Order *order_ptr = _head;
				if (_head == NULL)
				{
								Order *new_order = new Order(order);
								return new_order;
				};
				while ( order_ptr != NULL )
				{
								if (!can_trade(order_ptr, target_price, target_op))
								{
												order_ptr = order_ptr->next;
												continue;
								};
								long diff_quant = order_ptr->quant - target_quant;
								if ( diff_quant == 0 )
								{
												trade(order_ptr, order, target_quant);
												//order_ptr = order_ptr->next;
												remove_node(order_ptr);
												return NULL;
								}else if (diff_quant > 0)
								{
												trade(order_ptr, order, target_quant);
												order_ptr->quant = diff_quant;
												return NULL;
												
								}else{
												trade(order_ptr, order, order_ptr->quant);
												Order *tmp = order_ptr->next;
												remove_node(order_ptr);
												order_ptr = tmp;
												target_quant = -diff_quant;
												continue;
								}

				};
				if (target_quant != 0 )
				{
								Order *new_order = new Order(order);
								new_order->quant = target_quant;
								return new_order;
				}
				return NULL;

};
void MatchingEngine::buy_sell(const Order &order, const TYPE type)
{
				Order *remain;
				if ((remain = match(order)) != NULL && type == GFD) 
				{
					add_node(remain);
				};
};
void MatchingEngine::cancel(const Order &order)
{
				auto found = order_lookup.find(order.oid);
				if ( found != order_lookup.end() )
				{
								remove_node(found->second);

				}
				return;
};
void MatchingEngine::modify(const Order &order)
{
				cancel(order);
				buy_sell(order, GFD);
};
void MatchingEngine::add_map(DESC_MAP& _map, Order * const order_ptr)
{
				auto found = _map.find(order_ptr->price);
				if ( found == _map.end() )
				{
								_map[order_ptr->price] = order_ptr->quant;
				}else
				{
								_map[order_ptr->price] += order_ptr->quant;
				}
};
void MatchingEngine::print_map(DESC_MAP& _map)
{
				for (auto p : _map )
				{
								cout << p.first << " " << p.second << endl;
				};
}
void MatchingEngine::print()
{
				Order *order_ptr = _head;
				DESC_MAP buy_map, sell_map;
				while (order_ptr != NULL)
				{
								order_ptr->op == BUY ? add_map(buy_map, order_ptr) : add_map(sell_map, order_ptr);
								order_ptr = order_ptr->next;
				};
				cout << "SELL:" << endl;
				print_map(sell_map);
				cout << "BUY:" << endl;
				print_map(buy_map);
};

int main()
{
				MatchingEngine engine;
				string one_line;
				while ( getline(cin, one_line) )
				{
								engine.parse(one_line);
				};
				
				/*vector<string> inputs;
				inputs.push_back("BUY GFD 1000 30 order1");
				inputs.push_back("SELL IOC 900 70 order2");
				inputs.push_back("PRINT");
				for (auto p: inputs)
				{
								engine.parse(p);
				}*/
				
};
