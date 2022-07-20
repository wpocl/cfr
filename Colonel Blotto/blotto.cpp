#include <map>
#include <vector>
#include <random>
#include "partitions.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

int signum(int x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

class CBTrainer {
    public:
        static std::random_device seed;
        static std::mt19937_64 engine;
        int S, N, num_actions;
        std::vector<double> regret_sum;
        std::vector<double> strategy;
        std::vector<double> strategy_sum;
        std::map<int, std::vector<int>> action_map;
        std::unordered_map<int, std::unordered_map<int, int>> payoff_matrix;

        CBTrainer(int soldiers, int battlefields) {
            action_map = partitions(soldiers, battlefields);
            num_actions = action_map.size();
            S = soldiers;
            N = battlefields;
            regret_sum.resize(num_actions, 0.0);
            strategy.resize(num_actions, 0.0);
            strategy_sum.resize(num_actions, 0.0);
        }

        void compute_strategy() {
            double normalizing_sum = 0; 
            for (int a = 0; a < num_actions; a++) {
                strategy[a] = (regret_sum[a] > 0) ? regret_sum[a] : 0;
                normalizing_sum += strategy[a];
            }
            for (int a = 0; a < num_actions; a++) {
                if (normalizing_sum > 0)
                    strategy[a] /= normalizing_sum;
                else
                    strategy[a] = 1.0/num_actions;
                strategy_sum[a] += strategy[a];
            }
        }

        int compute_action() {
            std::discrete_distribution<size_t> dist(strategy.begin(), strategy.end());
            return dist(engine);
        }

        void update_regret(int action, int opp_action) {
            for (int a = 0; a < num_actions; a++) 
                regret_sum[a] += payoff_matrix[a][opp_action] - payoff_matrix[action][opp_action];
        }

        void compute_payoff_matrix(CBTrainer& opp_player) {
            for (int a = 0; a < num_actions; a++) {
            
                for (int opp_a = 0; opp_a < opp_player.num_actions; opp_a++) {
                    payoff_matrix[a][opp_a] = 0;
                    for (int n = 0; n < N; n++)
                        payoff_matrix[a][opp_a] += signum(action_map[a][n] - opp_player.action_map[opp_a][n]);
                    payoff_matrix[a][opp_a] = signum(payoff_matrix[a][opp_a]);
                }
            }
        }
        static void train(int iterations, CBTrainer& player0, CBTrainer& player1) {
            player0.compute_payoff_matrix(player1);
            player1.compute_payoff_matrix(player0);
            for (int i = 0; i < iterations; i++) {
                player0.compute_strategy();
                player1.compute_strategy();

                int action0 = player0.compute_action();
                int action1 = player1.compute_action();

                player0.update_regret(action0, action1);
                player1.update_regret(action1, action0);
                
                std::cout.precision(4);
                std::cout << std::fixed;
                if ((i+1) % 100 == 0) {
                    std::cout<<"iteration: "<<i+1<<std::endl;
                    std::cout<<std::setw(15)<<"P"<<std::setw(20)<<"Strategy"<<std::endl;
                    std::cout<<"Player 0"<<std::endl;
                    player0.print_average_strategy(i+1);
                    std::cout<<"Player 1"<<std::endl;
                    player1.print_average_strategy(i+1);
                }    
            }
        }

        void print_average_strategy(int iterations) {
            for (int i = 0; i < num_actions; i++) {
                std::cout<<std::setw(18)<<strategy_sum[i]/iterations<<std::setw(10)<<"[";
                for (int j = 0; j < N; j++) {
                    std::cout<<action_map[i][j];
                    if (j+1 != N)
                        std::cout<<", ";
                    else
                        std::cout<<"]";
                }
                std::cout<<std::endl;
            }
        }
};

std::random_device CBTrainer::seed;
std::mt19937_64 CBTrainer::engine(CBTrainer::seed());

int main() {
    CBTrainer player0(3, 8);
    CBTrainer player1(3, 8);

    CBTrainer::train(10000, player0, player1);
    return 0;
}