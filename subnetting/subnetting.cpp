#include <iostream>
#include <math.h>

/*
 * This file contains very easy to understand algorithms
 * that deal with subnetting.
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

  cout << "GSubnet: str_addr: " << str_addr << endl;
  cout << "GSubnet: str_mask: " << str_mask << endl;
  cout << "GSubnet: subnet: " << str_subnet << endl;

  cout << "GSubnet: addr: " << addr << endl;
  cout << "GSubnet: uint subnet: " << std::hex << subnet << endl;
  cout << "GSubnet: mask: " << mask << endl;

  return str_subnet;
}

string get_broadcast_addr(string str_addr, string str_mask) {
  auto subnet = get_subnet(str_addr, str_mask);
  auto subnet_uint = get_uint_version(subnet);

  auto mask_uint = get_uint_version(str_mask);

  auto broadcast_addr = subnet_uint | (~mask_uint);

  cout << "GBA: subnet_uint: " << std::hex << subnet_uint << endl;
  cout << "GBA: mask_uint: " << std::hex << mask_uint << endl;
  cout << "GBA broadcast_addr: " << get_string_version(broadcast_addr) << endl;

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
  cout << "GFVH: subnet: " << subnet << endl;

  auto first_valid_host = get_uint_version(subnet) + 1;
  return get_string_version(first_valid_host);
}

string get_last_valid_host(string str_addr, string str_mask) {
  auto broadcast_addr = get_broadcast_addr(str_addr, str_mask);
  auto last_valid_host = get_uint_version(broadcast_addr) - 1;

  cout << "GLVH: broadcast_addr: " << broadcast_addr << endl;

  return get_string_version(last_valid_host);
}

int how_many_host_bits(string str_mask) {
  int host_bits = 0;
  auto mask = get_uint_version(str_mask);
  while( ((mask>>host_bits) & 0x1) == false) {
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
  auto str_mask_num = cidr.substr(pos + 1);
  auto P = stoi(str_mask_num);
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

  cout << "GC: first_octet: " << first_octet << endl;

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

int get_p_from_str_mask(string str_mask) {
  auto host_bits = how_many_host_bits(str_mask);
  return 32-host_bits;
}

int how_many_subnets(string str_addr, string str_mask) {
  auto N = get_network_bits_from_class(get_class(str_addr));
  cout << "HMS: N:" << N << endl;

  auto P = get_p_from_str_mask(str_mask);
  cout << "HMS: P:" << P << endl;

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
  int x = 0;

  friend ostream &operator<<(ostream &os, const FL &me) {
    os << "X: " << me.x << endl;
    os << "First: " << me.first << ", Last: " << me.last << endl;
    return os;
  }
};

FL get_valid_host_range(string cidr) {
  auto str_addr = get_str_addr_from_cidr(cidr);
  auto str_mask = get_str_mask_from_cidr(cidr);

  cout << "GVHR: str_addr: " << str_addr << endl;
  cout << "GVHR: str_mask: " << str_mask << endl;

  FL fl = {get_first_valid_host(str_addr, str_mask),
           get_last_valid_host(str_addr, str_mask), 25};

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

int main() {

  uint32_t addr;
  string str_addr;

  uint32_t mask;
  string str_mask;

  string cidr;
 
  int num_subs;
  int num_hosts;

  str_addr = "172.31.0.0";
  str_mask = "255.255.254.0";

  cidr = "10.0.0.0/20";
  
  num_subs = 50;
  num_hosts = 600;

  // auto Ans1 = get_subnet(str_addr, str_mask);
  // auto Ans1 = get_subnet_from_cidr(str_addr);
  // auto Ans1 = get_broadcast_addr(str_addr, str_mask);
  // auto Ans1 = get_broadcast_addr_from_cidr(cidr);
  // auto Ans1 = get_first_valid_host(str_addr, str_mask);
  // auto Ans1 = get_last_valid_host(str_addr, str_mask);
  // auto Ans1 = get_subnet_from_host(str_addr);
  // auto Ans1 = get_valid_host_range(str_addr);
  auto Ans1 = how_many_subnets(str_addr, str_mask);
  auto Ans2 = how_many_hosts(str_mask);
  // auto Ans1 = how_many_subnets_from_cidr(cidr);
  // auto Ans2 = how_many_hosts_from_cidr(cidr);
  // auto Ans1 = get_subnet_mask_for_sh(str_addr, num_subs, num_hosts);

  cout << endl;
  cout << "Ans1: " << Ans1 << endl;
  cout << "Ans2: " << Ans2 << endl;

  return 0;
}
