import pyfi
from pyfi import brownian


def main() -> None:
    print("pyfi module:", pyfi)
    print("pyfi.brownian submodule:", brownian)
    print()

    # Create a random engine with fixed seed for reproducibility
    rng = brownian.RandomEngine(42)
    print("Created RandomEngine with seed 42")
    print()

    # 1) simulate_brownian_motion
    bm_path = brownian.simulate_brownian_motion(
        x0=0.0,
        T=1.0,
        steps=100,
        engine=rng,
    )
    print("simulate_brownian_motion (len, first, last):", 
          len(bm_path), bm_path[0], bm_path[-1])

    # 2) simulate_brownian_paths
    bm_paths = brownian.simulate_brownian_paths(
        x0=0.0,
        T=1.0,
        steps=50,
        num_paths=10,
        engine=rng,
    )
    print("simulate_brownian_paths (num_paths, path_len, first_path_last):", 
          len(bm_paths), len(bm_paths[0]), bm_paths[0][-1])

    # 3) simulate_gbm
    gbm_path = brownian.simulate_gbm(
        mu=0.05,
        sigma=0.2,
        s0=100.0,
        T=1.0,
        steps=252,
        engine=rng,
    )
    print("simulate_gbm (len, first, last):", 
          len(gbm_path), gbm_path[0], gbm_path[-1])

    # 4) gbm_mean
    mean = brownian.gbm_mean(mu=0.05)
    print("gbm_mean:", mean)

    # 5) gbm_variance
    variance = brownian.gbm_variance(sigma=0.2)
    print("gbm_variance:", variance)

    # 6) euler_maruyama - deterministic case (zero diffusion)
    drift_func = lambda x, t: 0.1 * x
    diffusion_func = lambda x, t: 0.0
    
    em_path_deterministic = brownian.euler_maruyama(
        drift=drift_func,
        diffusion=diffusion_func,
        x0=1.0,
        T=1.0,
        steps=1000,
        engine=rng,
    )
    print("euler_maruyama (deterministic, len, first, last):", 
          len(em_path_deterministic), em_path_deterministic[0], em_path_deterministic[-1])

    # 7) euler_maruyama - Ornstein-Uhlenbeck process
    # dX = -theta*(X - mu)*dt + sigma*dW
    theta = 0.5
    mu_ou = 2.0
    sigma_ou = 0.1
    
    drift_ou = lambda x, t: -theta * (x - mu_ou)
    diffusion_ou = lambda x, t: sigma_ou
    
    em_path_ou = brownian.euler_maruyama(
        drift=drift_ou,
        diffusion=diffusion_ou,
        x0=0.0,
        T=10.0,
        steps=1000,
        engine=rng,
    )
    print("euler_maruyama (Ornstein-Uhlenbeck, len, first, last):", 
          len(em_path_ou), em_path_ou[0], em_path_ou[-1])

    # 8) euler_maruyama - standard Brownian motion
    drift_zero = lambda x, t: 0.0
    diffusion_one = lambda x, t: 1.0
    
    em_path_bm = brownian.euler_maruyama(
        drift=drift_zero,
        diffusion=diffusion_one,
        x0=0.0,
        T=1.0,
        steps=100,
        engine=rng,
    )
    print("euler_maruyama (Brownian motion, len, first, last):", 
          len(em_path_bm), em_path_bm[0], em_path_bm[-1])

    # 9) Test reproducibility with reseed
    rng.reseed(123)
    path1 = brownian.simulate_brownian_motion(0.0, 1.0, 10, rng)
    
    rng.reseed(123)
    path2 = brownian.simulate_brownian_motion(0.0, 1.0, 10, rng)
    
    paths_equal = path1 == path2
    print("Reproducibility test (paths equal after reseed):", paths_equal)

    # 10) Multiple GBM paths for Monte Carlo
    rng_mc = brownian.RandomEngine(999)
    mc_paths = []
    for _ in range(1000):
        path = brownian.simulate_gbm(0.05, 0.2, 100.0, 1.0, 252, rng_mc)
        mc_paths.append(path[-1])
    
    mean_terminal = sum(mc_paths) / len(mc_paths)
    print("Monte Carlo GBM (1000 paths, mean terminal value):", mean_terminal)

    print()
    print("All brownian functions executed successfully!")


if __name__ == "__main__":
    main()