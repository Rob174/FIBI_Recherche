from FIBI.analyse_results.utils.__init__ import *
from scipy.stats import norm, linregress, probplot

"""--------------------------------------- QQPLOT ---------------------------------------"""

def _ppoints(n, a=0.5):
    """
    Taken from https://github.com/raphaelvallat/pingouin/blob/06801b8719ea73608e958b4e2d1734b0d5d3049b/pingouin/plotting.py
    """
    a = 3 / 8 if n <= 10 else 0.5
    return (np.arange(n) + 1 - a) / (n + 1 - 2 * a)

def qqplot(x, confidence=0.95):
    if np.max(np.abs(x)) < 1e-6:
        raise Exception("No need to plot qqplot for this data as there is no difference between the two distributions")
    if np.std(x) < 1e-6:
        raise Exception("No need to plot qqplot for this data as the two distributions have a constant difference")
    # Taken from https://github.com/raphaelvallat/pingouin/blob/06801b8719ea73608e958b4e2d1734b0d5d3049b/pingouin/plotting.py
    x = x[~np.isnan(x)]
    if len(x) == 0:
        return False
    # Extract quantiles and regression
    quantiles = probplot(x, dist=norm, fit=False)  # type: ignore # type: ignore
    theor, observed = quantiles[0], quantiles[1]

    fit_params = norm.fit(x)
    loc = fit_params[-2]
    scale = fit_params[-1]
    shape = fit_params[:-2] if len(fit_params) > 2 else None

    # Observed values to observed quantiles
    if loc != 0 and scale != 1:
        observed = (np.sort(observed) - loc) / scale

    # Linear regression
    slope, intercept, r, _, _ = linregress(theor, observed)

    # Add diagonal line
    end_pts = [[np.min(theor), np.max(theor)], [np.min(observed), np.max(observed)]]
    end_pts[0] = min(end_pts[0])
    end_pts[1] = max(end_pts[1])

    fit_val = slope * theor + intercept
    # Confidence envelope
    n = x.size
    P = _ppoints(n)
    crit = norm.ppf(1 - (1 - confidence) / 2)
    pdf = norm.pdf(theor) if shape is None else norm.pdf(theor, *shape)
    se = (slope / pdf) * np.sqrt(P * (1 - P) / n)
    upper = fit_val + crit * se
    lower = fit_val - crit * se
    return theor, fit_val, observed, upper, lower

"""------------------------------------- QQPLOT END ---------------------------------------"""

def check_in_inter(theor, fit_val, upper, lower) -> Literal["ng", "gh", "g"]:
    count_out_tail = 0
    for t, v, u, l in zip(theor, fit_val, upper, lower):
        if abs(t) > 1.5 and (v > u or v < l):
            count_out_tail += 1
        elif abs(t) <= 1.5 and v > u or v < l:
            return "ng"
    if count_out_tail < 5:
        return "g"
    else:
        return "gh"