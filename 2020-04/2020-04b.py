import argparse
import collections
import re
from typing import Dict, List

RawPassportList = List[str]
Passport = Dict[str, str]

field_pattern = re.compile(R'(\S+?:\S+)')
height_pattern = re.compile(R'(\d+)(cm|in)')
hair_colour_pattern = re.compile(R'#[0-9a-f]{6}')
eye_colour_pattern = re.compile(R'amb|blu|brn|gry|grn|hzl|oth')
passport_id_pattern = re.compile(R'^\d{9}$')

def validate_year(input: str, min_year: int, max_year: int) -> bool:
    return len(input) == 4 and int(input) >= min_year and int(input) <= max_year

def validate_birth_year(input: str) -> bool:
    return validate_year(input, 1920, 2002)

def validate_issue_year(input: str) -> bool:
    return validate_year(input, 2010, 2020)

def validate_expiration_year(input: str) -> bool:
    return validate_year(input, 2020, 2030)

def validate_height(input: str) -> bool:
    match = height_pattern.match(input)
    if not match:
        return False
    height_value = int(match.group(1))
    suffix = match.group(2)
    if suffix == 'cm':
        return height_value >= 150 and height_value <= 193
    elif suffix == 'in':
        return height_value >= 59 and height_value <= 76
    return False

def validate_hair_colour(input: str) -> bool:
    return hair_colour_pattern.match(input)

def validate_eye_colour(input: str) -> bool:
    return eye_colour_pattern.match(input)

def validate_passport_id(input: str) -> bool:
    return passport_id_pattern.match(input)

required_fields = {
    'byr': validate_birth_year, 'iyr': validate_issue_year,
    'eyr': validate_expiration_year, 'hgt': validate_height,
    'hcl': validate_hair_colour, 'ecl': validate_eye_colour,
    'pid': validate_passport_id}


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
        if all(field in passport.keys() and validator(passport[field]) for (field, validator) in required_fields.items()):
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
