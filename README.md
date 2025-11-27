# PyFi - Python Financial Instruments Library

PyFi is a high-performance Python library for financial instrument pricing and analysis, built with C++ and exposed
through Python bindings. It provides fast, accurate implementations of bond pricing models, option pricing models (
including Black-Scholes and binomial trees), the Greeks for risk management, and stochastic process simulations
including standard Brownian motion and Geometric Brownian Motion (GBM).

## Authors

- **Nikolay Tsonev** - nikolay.l.tsonev@gmail.com
- **Bilal Waraich** - bilal.waraich2004@outlook.com

## Features

### Bond Pricing Module

- Present value calculations with discrete compounding
- Internal rate of return (yield-to-maturity) calculations
- Zero-coupon and coupon bond pricing with fractional maturity support
- Clean and dirty price calculations
- Accrued interest computation
- Forward value calculations

### Options Pricing Module

- **European Options**: Black-Scholes pricing for calls and puts
- **American Options**: Binomial tree pricing with early exercise
- **Greeks**: Delta, Gamma, Theta, Vega, and Rho
- Forward pricing and implied dividend yield calculations
- Support for continuous dividend yields

### Stochastic Processes Module (In Development)

- **Standard Brownian Motion**: Simulation of Wiener processes
- **Geometric Brownian Motion (GBM)**: Stock price simulation and Monte Carlo methods
- Applications in option pricing and risk modeling

## Requirements

### System Requirements

- C++20 compatible compiler
- CMake 3.20 or higher
- Python 3.8 or higher
- Boost library (for mathematical utilities)

### Python Requirements

- setuptools >= 60.0
- wheel
- pybind11 >= 2.6

## Installation

### 1. Clone the Repository

```bash
git clone <your-repo-url>
cd PyFi
```

### 2. Install Dependencies

#### On Ubuntu/Debian:

```bash
sudo apt-get update
sudo apt-get install cmake build-essential libboost-all-dev python3-dev
```

#### On macOS:

```bash
brew install cmake boost python3
```

#### On Windows:

Install CMake from the official website and Boost from vcpkg or the official installer.

### 3. Build the C++ Library

Create a build directory and compile:

```bash
mkdir build
cd build
cmake ..
cmake --build . -j $(nproc)
cd ..
```

This will:

- Fetch and build pybind11 and Catch2 (for tests)
- Compile the PyFi C++ library
- Create the Python extension module `_pyfi.so` (or `.pyd` on Windows)
- Copy the shared library to the `pyfi/` directory

### 4. Install into Python Environment

It's recommended to use a virtual environment:

```bash
# Create and activate a virtual environment
python3 -m venv .venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate
```

Install PyFi in development mode (editable install):

```bash
pip install -e .
```

Or for a standard installation:

```bash
pip install .
```

**Note**: The editable install (`-e`) is useful during development as it allows changes to be reflected without
reinstalling.

## Quick Start

### Bond Pricing Example

```python
from pyfi import bond

# Bond parameters
par_value = 1000.0  # Face value
coupon_rate = 0.05  # 5% annual coupon
annual_yield = 0.04  # 4% yield to maturity
years = 5  # 5 years to maturity
m = 2  # Semiannual compounding

# Calculate present value
cash_flows = bond.build_bond_cashflows(
    par_value=par_value,
    coupon_rate=coupon_rate,
    years=years,
    m=m
)

pv = bond.present_value(
    cash_flows=cash_flows,
    annual_yield=annual_yield,
    par_value=par_value,
    years=years,
    compounding_annually=m,
    same_cashflows=False
)

print(f"Present Value: ${pv:.2f}")

# Calculate bond price from yield
price = bond.price_from_yield(
    cash_flows=cash_flows,
    annual_yield=annual_yield,
    m=m
)

print(f"Bond Price: ${price:.2f}")

# Calculate yield to maturity (internal rate of return)
ytm = bond.internal_rate_return(
    cash_flows=cash_flows,
    price=price,
    interest_rate=coupon_rate,
    par_value=par_value,
    years=years,
    compounding_annually=m
)

print(f"Yield to Maturity: {ytm * 100:.4f}%")

# Calculate clean and dirty prices
years_to_maturity = 3.75
clean_price = bond.clean_coupon_price_from_T(
    par_value=par_value,
    coupon_rate=coupon_rate,
    annual_yield=annual_yield,
    years_to_maturity=years_to_maturity,
    m=m
)

dirty_price = bond.dirty_coupon_price_from_T(
    par_value=par_value,
    coupon_rate=coupon_rate,
    annual_yield=annual_yield,
    years_to_maturity=years_to_maturity,
    m=m
)

print(f"Clean Price: ${clean_price:.2f}")
print(f"Dirty Price: ${dirty_price:.2f}")
```

### Option Pricing Example

```python
from pyfi import option

# Option parameters
stock_price = 100.0  # Current stock price
strike_price = 100.0  # Strike price
volatility = 0.2  # 20% annual volatility
risk_free_rate = 0.05  # 5% risk-free rate
time_to_maturity = 1.0  # 1 year
dividend_yield = 0.02  # 2% dividend yield

# European option pricing (Black-Scholes)
call_price = option.black_scholes_call(
    stock_price=stock_price,
    strike_price=strike_price,
    volatility=volatility,
    risk_free_rate=risk_free_rate,
    time=time_to_maturity,
    yield_curve=dividend_yield
)

put_price = option.black_scholes_put(
    stock_price=stock_price,
    strike_price=strike_price,
    volatility=volatility,
    risk_free_rate=risk_free_rate,
    time=time_to_maturity,
    yield_curve=dividend_yield
)

print(f"European Call Price: ${call_price:.4f}")
print(f"European Put Price: ${put_price:.4f}")

# Calculate the Greeks
delta = option.bs_call_delta(
    stock_price=stock_price,
    strike_price=strike_price,
    volatility=volatility,
    risk_free_rate=risk_free_rate,
    dividend_yield=dividend_yield,
    time=time_to_maturity
)

gamma = option.bs_gamma(
    stock_price=stock_price,
    strike_price=strike_price,
    volatility=volatility,
    risk_free_rate=risk_free_rate,
    dividend_yield=dividend_yield,
    time=time_to_maturity
)

vega = option.bs_vega(
    stock_price=stock_price,
    strike_price=strike_price,
    volatility=volatility,
    risk_free_rate=risk_free_rate,
    dividend_yield=dividend_yield,
    time=time_to_maturity
)

print(f"Delta: {delta:.4f}")
print(f"Gamma: {gamma:.4f}")
print(f"Vega: {vega:.4f}")

# American option pricing (Binomial Tree)
steps = 200  # Number of time steps

american_call = option.binomial_us_option(
    stock_price=stock_price,
    strike_price=strike_price,
    volatility=volatility,
    risk_free_rate=risk_free_rate,
    steps=steps,
    time=time_to_maturity,
    payoff_type="call"
)

american_put = option.binomial_us_option(
    stock_price=stock_price,
    strike_price=strike_price,
    volatility=volatility,
    risk_free_rate=risk_free_rate,
    steps=steps,
    time=time_to_maturity,
    payoff_type="put"
)

print(f"American Call Price: ${american_call:.4f}")
print(f"American Put Price: ${american_put:.4f}")
```

## Running Tests

The library includes comprehensive C++ unit tests using Catch2:

```bash
cd build
ctest --output-on-failure
```

Or run tests directly:

```bash
./build/test/test_bond
./build/test/test_option
```

Python tests can be run from the `test/python_test/` directory:

```bash
python test/python_test/test_bond.py
python test/python_test/test_option.py
```

## API Documentation

### Bond Module (`pyfi.bond`)

Key functions:

- `present_value()` - Calculate present value of cash flows
- `internal_rate_return()` - Calculate yield to maturity
- `build_bond_cashflows()` - Generate standard bond cash flow schedule
- `price_from_yield()` - Calculate bond price from yield
- `zero_coupon_price()` - Price zero-coupon bonds
- `coupon_bond_price()` - Price coupon-bearing bonds
- `clean_coupon_price()` - Calculate clean (quoted) price
- `dirty_coupon_price()` - Calculate dirty (invoice) price
- `accrued_interest()` - Calculate accrued interest
- `forward_value()` - Calculate forward price

### Option Module (`pyfi.option`)

Key functions:

- `black_scholes_call()` - European call option price
- `black_scholes_put()` - European put option price
- `binomial_eu_option()` - European option via binomial tree
- `binomial_us_option()` - American option via binomial tree
- `bs_call_delta()`, `bs_put_delta()` - Option delta
- `bs_gamma()` - Option gamma
- `bs_call_theta()`, `bs_put_theta()` - Option theta
- `bs_vega()` - Option vega
- `bs_call_rho()`, `bs_put_rho()` - Option rho
- `forward_from_yield()` - Calculate forward price from dividend yield
- `yield_from_forward()` - Calculate implied dividend yield

### Stochastic Processes Module (`pyfi.brownian`) - Coming Soon

Planned functions:

- Brownian motion path simulation
- Geometric Brownian Motion (GBM) for stock price modeling
- Monte Carlo simulation utilities
- Variance reduction techniques

**Note**: The stochastic processes module is currently under development. Check the project repository for updates.

## Project Structure

```
PyFi/
├── include/pyfi/          # C++ header files
│   ├── bond.h            # Bond pricing declarations
│   └── option.h          # Option pricing declarations
├── src/                   # C++ implementation
│   ├── bond.cpp
│   ├── option.cpp
│   └── option_greeks.cpp
├── pybind/               # Python binding code
│   ├── pyfi_bind.cpp
│   ├── bond_bind.cpp
│   └── option_bind.cpp
├── pyfi/                 # Python package
│   └── __init__.py
├── test/                 # C++ and Python tests
│   ├── test_bond.cpp
│   ├── test_option.cpp
│   └── python_test/
├── CMakeLists.txt        # Main CMake configuration
├── pyproject.toml        # Python package metadata
└── README.md             # This file
```

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch
3. Write tests for new functionality
4. Ensure all tests pass
5. Format code using clang-format (config provided)
6. Submit a pull request

## License

[Specify your license here]

## Acknowledgments

- Built with [pybind11](https://github.com/pybind/pybind11) for Python bindings
- Uses [Boost](https://www.boost.org/) for mathematical utilities
- Testing with [Catch2](https://github.com/catchorg/Catch2)

## Support

For issues, questions, or contributions, please contact the authors or open an issue on the project repository.