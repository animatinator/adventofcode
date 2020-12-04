import argparse
import collections
import re
from typing import Dict, List

RawPassportList = List[str]
Passport = Dict[str, str]

field_pattern = re.compile(R'(\S+?:\S+)')
required_fields = ['byr', 'iyr', 'eyr', 'hgt', 'hcl', 'ecl', 'pid']


def parse_passport(raw_passport: str) -> Passport:
    result = collections.defaultdict()
    field_matches = re.findall(field_pattern, raw_passport)
    for match in field_matches:
        (key, value) = match.split(':')
        result[key] = value
    return result


def count_valid_passports(passports: RawPassportList) -> int:
    count = 0
    for raw_passport in passports:
        passport = parse_passport(raw_passport)
        if all(field in passport.keys() for field in required_fields):
            count += 1
    return count


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Solve Advent of Code day 4.')
    parser.add_argument('input_file', type=str, help='The file to process')
    args = parser.parse_args()
    
    content = open(args.input_file, 'r')
    data = ''.join(content.readlines()).split('\n\n')

    valid_count = count_valid_passports(data)
    print(f"Found {valid_count} valid passports in the input data.")
