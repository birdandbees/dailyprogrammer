//
// https://www.reddit.com/r/dailyprogrammer/comments/3xdmtw/20151218_challenge_245_hard_guess_whois/
//
#include <vector>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iostream>
using namespace std;
using namespace boost;

class IP
{
	friend vector<int> parse_ip(string ip);
	public:
	string ip;
	double hash;
	IP(string ip_str);
	inline IP(){};
	static bool compare(const IP& one, const IP& two);
};

IP::IP(string group)
{
	ip = group;
	vector<string> splitted;
	hash = 0;
	boost::split(splitted, ip, boost::is_any_of("."));
	for (auto segment : splitted)
	{
		hash += hash * 1000 + stoi(segment);
	};
};

bool IP::compare(const IP& one , const IP& two)
{
	return one.hash < two.hash;
}

vector<int> parse_ip(string ip)
{
	vector<string> splitted;
	vector<int> result;
	boost::split(splitted, ip, boost::is_any_of("."));
	for (auto segment : splitted)
	{
		result.push_back(stoi(segment));
	};
	return result;
}

struct Srange
{
	string lower;
	string upper;
	friend bool compare(Srange& r1, Srange& r2);
};

bool compare(Srange& r1, Srange& r2)
{
	IP ip1 = r1.lower;
	IP ip2 = r2.lower;
	return IP::compare(ip1, ip2);
}

class TreeNode 
{
	public:
	IP lower;
	IP upper;
	TreeNode * left;
	TreeNode * right;
	IP max;

	TreeNode(string lower, string upper);
	TreeNode& seach(TreeNode* root, IP& target);
	friend vector<int> parse_ip(string ip);
  bool isLeaf();
  bool isIntersect(IP& target);
  static TreeNode* construct(vector<Srange>& ips, size_t begin, size_t end);
  static TreeNode* search(TreeNode & root, IP& target);
};

TreeNode::TreeNode(string lower_str, string upper_str)
{
	left = nullptr;
	right = nullptr;
	lower = lower_str; 
	upper = upper_str;
	max = upper_str;
}

/*TreeNode * TreeNode::add(string ip, TreeNode * root)
	{
	TreeNode newNode{ip};
	if (root == NULL ) return &newNode;

	}*/

bool TreeNode::isLeaf()
{
	return left == NULL && right == NULL;
}

bool TreeNode::isIntersect(IP& target)
{
	return  target.hash >= lower.hash && target.hash <= upper.hash;
}

TreeNode * TreeNode::construct(vector<Srange>& ips, size_t begin, size_t end)
{
	size_t idx = begin + (end - begin + 1 ) / 2;
	TreeNode *  newNode = new TreeNode(ips.at(idx).lower, ips.at(idx).upper);
	if ( begin == end )
	{
		return newNode;
	};

	TreeNode* left = construct(ips, 0, idx - 1);
	TreeNode* right = construct(ips, idx + 1, ips.size());
	newNode->left = left;
	newNode->right = right;
	newNode->max = left->max.hash > right->max.hash ? left->max : right->max ;
	return newNode;
}

TreeNode* search(TreeNode* root, IP& target)
{
	//search until leaf node is reached
	TreeNode * current = root;
	bool found;
	while (!current->isLeaf())
	{
		found = current->isIntersect(target);
		if ( target.hash > current->lower.hash )
		{
			current = current->right;
		}else
		{
			current = current->left;
		};
	}
}

int main()
{
	Srange r1 = {"123.45.100.0", "123.60.32.1"};
	Srange r2 = {"123.45.67.8", "123.45.123.45"};
	Srange r3 = {"123.50.10.1", "123.50.1.1"};
	Srange r4 = {"200.0.0.250", "188.0.0.3"};

	vector<Srange> ips = {r1, r2, r3, r4};
	std::sort(ips.begin(), ips.end(), compare);
	TreeNode * root = TreeNode::construct(ips, 0, 3);
	std::cout << " debugging " << endl;

	return 0;
};


