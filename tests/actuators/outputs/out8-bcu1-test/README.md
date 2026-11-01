# out8 host tests

Build out8-bcu1-test_x64 in Debug with the fixed sblib checkout, then run CTest.
All six raw application cases, including Logic, must pass. CTest runs those six
cases directly and one complete shared-process suite; no expected failure adapter.
Crashes, missing binaries, infrastructure errors and nonzero exits fail normally.

Every case uses Out8TestFixture. Check raw declaration order and random seeds 1095
(reverse order) and 32 (Logic first). The delay test keeps its feedback/quiet checks.

The Logic scenario follows application 206001, Gira 10283110.pdf, pages 18 and 21:
AND with feedback resets a switching 1 received while the logic input is 0.
Raising the logic input alone cannot switch on; a fresh switching 1 is required.
The Jung ETS application identifies itself as 2060 version 1 and records original
manufacturer M-000A. The historical generated scenario violated this contract.
Product sources are unchanged. Additional matrix assertions exercise OR, AND and
AND with feedback in both input orders, including repeated values and stored state.

Regenerate only the Logic telegram array with Python 3:
`python regenerate_logic_sequence.py --write`, or `--check` for byte equality.
This focused renderer consumes special_logic.tc, special_init.steps and its device
GA table. It does not regenerate EEPROM, locking data, fixtures or other cases.
