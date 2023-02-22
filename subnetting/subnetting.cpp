#include <iostream>
#include <math.h>

/*
 * This file contains very easy to understand algorithms
 * that deal with subnetting.
 *
 * Notation:
 *
 *   a. addr = 10.20.30.40 (represented in uint32_t)
 *   b. str_addr = "10.20.30.40"
 *   c. mask = 225.255.0.0 (represented in uint32_t)
 *   d. str_mask = "255.255.0.0"
 *   e. cidr = "10.20.30.40/16"
 *   e. cidr_mask = P = 16 (int)
 *   f. str_cidr_mask = "16"
 *   g. host_bits = H = 32 - P
 */

using namespace std;

string get_string_version(uint32_t addr) {
  string str;
  for (int i = 0; i < 4; i++) {
    str = to_string(((addr >> (i * 8)) & 0xFF)) + "." + str;
  }
  return str.substr(0, str.size() - 1);
}

uint32_t get_uint_version(string str) {
  uint32_t addr = 0;
  size_t it = 0;
  size_t prev_it = it;
  int shift = 3;

  do {
    it = str.find(".", prev_it + 1);
    auto block_str = str.substr(prev_it, it - prev_it);
    uint32_t block_int = stoi(block_str);
    if (block_int > 255)
      return -1;
    addr |= ((block_int) << (shift * 8));
    shift--;
    prev_it = it + 1;
  } while (it != string::npos);
  return addr;
}

string get_subnet(string str_addr, string str_mask) {
  auto addr = get_uint_version(str_addr);
  auto mask = get_uint_version(str_mask);
  auto subnet = addr & mask;
  auto str_subnet = get_string_version(subnet);
  return str_subnet;
}

string get_broadcast_addr(string str_addr, string str_mask) {
  auto subnet = get_subnet(str_addr, str_mask);
  auto subnet_uint = get_uint_version(subnet);
  auto mask_uint = get_uint_version(str_mask);
  auto broadcast_addr = subnet_uint | (~mask_uint);
  return get_string_version(broadcast_addr);
}

string get_mask_for_p(int p) {
  auto shift = 32 - p;
  auto mask = ((0xFFFFFFFF >> shift) << shift);
  return get_string_version(mask);
}

string get_str_mask_from_cidr(string cidr) {
  auto pos = cidr.find("/");
  auto str_cidr_mask = cidr.substr(pos + 1);
  return get_mask_for_p(stoi(str_cidr_mask));
}

string get_str_addr_from_cidr(string cidr) {
  auto pos = cidr.find("/");
  return cidr.substr(0, pos);
}

string get_broadcast_addr_from_cidr(string cidr) {
  auto str_addr = get_str_addr_from_cidr(cidr);
  auto str_mask = get_str_mask_from_cidr(cidr);
  return get_broadcast_addr(str_addr, str_mask);
}

string get_first_valid_host(string str_addr, string str_mask) {
  auto subnet = get_subnet(str_addr, str_mask);
  auto first_valid_host = get_uint_version(subnet) + 1;
  return get_string_version(first_valid_host);
}

string get_last_valid_host(string str_addr, string str_mask) {
  auto broadcast_addr = get_broadcast_addr(str_addr, str_mask);
  auto last_valid_host = get_uint_version(broadcast_addr) - 1;
  return get_string_version(last_valid_host);
}

int how_many_host_bits(string str_mask) {
  int host_bits = 0;
  auto mask = get_uint_version(str_mask);
  while (((mask >> host_bits) & 0x1) == false) {
    host_bits++;
  }
  return host_bits;
}

int how_many_hosts(string str_mask) {
  auto host_bits = how_many_host_bits(str_mask);
  return pow(2, host_bits) - 2;
}

int how_many_hosts_from_cidr(string cidr) {
  auto pos = cidr.find("/");
  auto str_cidr_mask = cidr.substr(pos + 1);
  auto P = stoi(str_cidr_mask);
  return pow(2, 32 - P) - 2;
}

string get_subnet_from_cidr(string cidr) {
  auto str_addr = get_str_addr_from_cidr(cidr);
  auto str_mask = get_str_mask_from_cidr(cidr);
  return get_subnet(str_addr, str_mask);
}

char get_class(string addr) {
  auto pos = addr.find(".");
  auto first_octet = stoi(addr.substr(0, pos));
  if (first_octet < 128)
    return 'A';
  if (first_octet < 192)
    return 'B';
  if (first_octet < 224)
    return 'C';
  return 'D';
}

int get_network_bits_from_class(char cl) {
  if (cl == 'A')
    return 8;
  if (cl == 'B')
    return 16;
  if (cl == 'C')
    return 24;
  return 32;
}

int get_network_bits_from_str_addr(string str_addr) {
  return get_network_bits_from_class(get_class(str_addr));
}

int get_p_from_str_mask(string str_mask) {
  auto host_bits = how_many_host_bits(str_mask);
  return 32 - host_bits;
}

int how_many_subnets(string str_addr, string str_mask) {
  auto N = get_network_bits_from_class(get_class(str_addr));
  auto P = get_p_from_str_mask(str_mask);
  return pow(2, P - N);
}

int how_many_subnets_from_cidr(string cidr) {
  auto str_addr = get_str_addr_from_cidr(cidr);
  auto str_mask = get_str_mask_from_cidr(cidr);
  return how_many_subnets(str_addr, str_mask);
}

struct FL {
  string first;
  string last;

  friend ostream &operator<<(ostream &os, const FL &me) {
    os << "First: " << me.first << ", Last: " << me.last << endl;
    return os;
  }
};

FL get_valid_host_range(string cidr) {
  auto str_addr = get_str_addr_from_cidr(cidr);
  auto str_mask = get_str_mask_from_cidr(cidr);
  FL fl = {get_first_valid_host(str_addr, str_mask),
           get_last_valid_host(str_addr, str_mask)};

  return fl;
}

int get_subnet_bits_from_subnets(int subnets) {
  int bits = 0;
  while (pow(2, bits) < subnets) {
    bits++;
  }
  return bits;
}

string get_subnet_mask_for_sh(string str_addr, int num_subs, int num_hosts) {
  int N = get_network_bits_from_class(get_class(str_addr));
  int subnet_bits = get_subnet_bits_from_subnets(num_subs);
  int P = N + subnet_bits;
  return get_mask_for_p(P);
}

int get_num_hosts() {
  string nh;
  cout << "Enter number of hosts: " << endl;
  cin >> nh;
  return stoi(nh);
}

int get_num_subs() {
  string ns;
  cout << "Enter number of subs: " << endl;
  cin >> ns;
  return stoi(ns);
}

string get_cidr() {
  string cidr;
  cout << "Enter cidr: " << endl;
  cin >> cidr;
  return cidr;
}

string get_str_mask() {
  string str_mask;
  cout << "Enter str_mask: " << endl;
  cin >> str_mask;
  return str_mask;
}

string get_str_addr() {
  string str_addr;
  cout << "Enter str_addr: " << endl;
  cin >> str_addr;
  return str_addr;
}

int main() {


  int select_question;
  cout << "Select question from following:" << endl;
  cout << "  1) get_subnet(str_addr, str_mask)" << endl;
  cout << "  2) get_subnet_from_cidr(cidr)" << endl;
  cout << "  3) get_broadcast_addr(str_addr, str_mask)" << endl;
  cout << "  4) get_broadcast_addr_from_cidr(cidr)" << endl;
  cout << "  5) get_first_valid_host(str_addr, str_mask)" << endl;
  cout << "  6) get_last_valid_host(str_addr, str_mask)" << endl;
  cout << "  7) -NA-" << endl;
  cout << "  8) get_valid_host_range(str_addr)" << endl;
  cout << "  9) how_many_subnets(str_addr, str_mask)" << endl;
  cout << "  10) how_many_hosts(str_mask)" << endl;
  cout << "  11) how_many_subnets_from_cidr(cidr)" << endl;
  cout << "  12) how_many_hosts_from_cidr(cidr)" << endl;
  cout << "  13) get_subnet_mask_for_sh(str_addr, num_subs, num_hosts)" << endl;
  cout << endl;

  cin >> select_question;
  cout << endl;

  switch (select_question) {
  case 1: {
    auto str_addr = get_str_addr();
    auto str_mask = get_str_mask();
    cout << "\nAnswer is: " << get_subnet(str_addr, str_mask) << endl;
    break;
  }
  case 2: {
    auto str_addr = get_str_addr();
    cout << "\nAnswer is: " << get_subnet_from_cidr(str_addr) << endl;
    break;
  }
  case 3: {
    auto str_addr = get_str_addr();
    auto str_mask = get_str_mask();
    cout << "\nAnswer is: " << get_broadcast_addr(str_addr, str_mask) << endl;
    break;
  }
  case 4: {
    auto cidr = get_cidr();
    cout << "\nAnswer is: " << get_broadcast_addr_from_cidr(cidr) << endl;
    break;
  }
  case 5: {
    auto str_addr = get_str_addr();
    auto str_mask = get_str_mask();
    cout << "\nAnswer is: " << get_first_valid_host(str_addr, str_mask) << endl;
    break;
  }
  case 6: {
    auto str_addr = get_str_addr();
    auto str_mask = get_str_mask();
    cout << "\nAnswer is: " << get_last_valid_host(str_addr, str_mask) << endl;
    break;
  }

  // case 7:

  case 8: {
    auto str_addr = get_str_addr();
    cout << "\nAnswer is: " << get_valid_host_range(str_addr) << endl;
    break;
  }
  case 9: {
    auto str_addr = get_str_addr();
    auto str_mask = get_str_mask();
    cout << "\nAnswer is: " << how_many_subnets(str_addr, str_mask) << endl;
    break;
  }
  case 10: {
    auto str_mask = get_str_mask();
    cout << "\nAnswer is: " << how_many_hosts(str_mask) << endl;
    break;
  }
  case 11: {
    auto cidr = get_cidr();
    cout << "\nAnswer is: " << how_many_subnets_from_cidr(cidr) << endl;
    break;
  }
  case 12: {
    auto cidr = get_cidr();
    cout << "\nAnswer is: " << how_many_hosts_from_cidr(cidr) << endl;
    break;
  }
  case 13: {
    auto str_addr = get_str_addr();
    auto num_subs = get_num_subs();
    auto num_hosts = get_num_hosts();
    cout << "\nAnswer is: "
         << get_subnet_mask_for_sh(str_addr, num_subs, num_hosts) << endl;
    break;
  }

  default: {
    cout << "Invalid selection '" << select_question << "' provided." << endl;
    break;
  }
  }

  cout << endl;
  return 0;
}
