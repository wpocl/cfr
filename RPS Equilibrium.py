import numpy as np

def u(action_1, action_2):
    payoff = [[0, -1, 1], [1, 0, -1], [-1, 1, 0]]
    return payoff[action_1][action_2]
 
class RPSTrainer:
    def __init__(self):
        self.active_players = [0, 1]
        self.regret_sum = {0: np.zeros(3), 1: np.zeros(3)}
        self.strategy = {0: None, 1: None}
        self.action = {0: None, 1: None}
        self.strategy_sum = {0: np.zeros(3), 1: np.zeros(3)}
        self.iterations = 0
        
    def compute_strategies(self):
        for player_number in self.active_players:
            strategy = np.clip(self.regret_sum[player_number], 0, None)
            normalizing_sum = np.sum(strategy)

            if normalizing_sum != 0:
                strategy /= normalizing_sum
            else:
                n = strategy.size
                strategy = np.repeat(1/n, n)
            self.strategy[player_number] = strategy
            self.strategy_sum[player_number] += strategy
    
    def compute_actions(self):
        for player_number in range(2):
            self.action[player_number] = np.random.choice(a=list(range(3)), p=self.strategy[player_number])
            
    def compute_regrets(self):
        for player_number in self.active_players:
            other_player_number = (player_number+1)%2

            chosen_action_u = u(self.action[player_number], self.action[other_player_number])
            regret = [u(action, self.action[other_player_number]) - chosen_action_u for action in range(3)]

            self.regret_sum[player_number] += np.array(regret)
    
    def train(self, iterations, opp_strategy=None):
        self.iterations = iterations
        if opp_strategy != None:
            self.active_players.remove(1)
            self.strategy[1] = opp_strategy

        for _ in range(self.iterations):
            self.compute_strategies()
            self.compute_actions()
            self.compute_regrets()
    
    def compute_optimal_strategy(self):
        return self.strategy_sum[0]/self.iterations
    

if __name__ == "__main__":
    trainer = RPSTrainer()
    trainer.train(iterations=10000)
    print(trainer.compute_optimal_strategy())
