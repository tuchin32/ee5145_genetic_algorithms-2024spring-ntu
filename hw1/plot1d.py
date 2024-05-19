import numpy as np
import matplotlib.pyplot as plt

if __name__ == "__main__":
    l = np.arange(85)
    

    a = 1 - (1 - 0.005791)**(l * (l - 1) * (l - 2) / 6)
    b = 1 - (1 - 0.000286)**(l * (l - 1) * (l - 2) * (l - 3) / 24)

    fig, ax = plt.subplots()
    ax.grid()
    ax.plot(l, a, '.', label='$k = 3$')
    ax.plot(l, a, label=None, color='tab:blue', alpha=0.5)
    ax.plot(l, b, '.', label='$k = 4$')
    ax.plot(l, b, label=None, color='tab:orange', alpha=0.5)
    ax.legend()
    ax.set_xlabel('Problem size $ell$')
    ax.set_ylabel('Probability of $k$-deception')
    plt.savefig('./plot/p1-deception.png', dpi=300, bbox_inches='tight')

    print(np.where(a > 0.999))
    print(np.where(b > 0.999))

    # print(a)
    # print(b)