#include <iostream>
#include <vector>

using namespace std;

struct Stack;
struct Queue;

void TestQueue(int = 1e5);

int main() {
	TestQueue();
}

struct Stack {

	void push(int x) {
		st.push_back(x);
	}

	int top() {
		return st.back();
	}

	int pop() {
		int x = st.back();
		st.pop_back();

		return x;
	}

	int empty() {
		return st.empty();
	}

	size_t size() {
		return st.size();
	}

	vector<int> st;

};

#define MOVE_ELEM(st1, st2)	st2.push(st1.pop())

const int STEP_COUNT = 4;
enum Mode {NORMAL, BERSERK};
enum Phase {OUT, IN, TMP, CLEAN, NORM};

struct Queue {

	void push(int x) {
		if (mode == NORMAL) {
			in.push(x);

			CleanCopy();

			TurnOnIfNeed();
		}
		else {
			inCopy.push(x);
			MakeSteps(STEP_COUNT);
		}
	}

	int pop() {
		int res = 0;

		if (mode == NORMAL) {
			res = out.pop();
			outCopy.pop();

			CleanCopy();

			TurnOnIfNeed();
		}
		else {
			++deleted;
			res = outCopy.pop();
			MakeSteps(STEP_COUNT);
		}

		return res;
	}

	void TurnOnIfNeed() {
		if (in.size() > out.size()) {
			mode = BERSERK;
			phase = OUT;

			MakeSteps(STEP_COUNT);
			
			kol = (int)out.size();
			deleted = 0;
		}
	}

	void CleanCopy() {
		if (!outCopy2.empty())
			outCopy2.pop();
	}

	void MakeSteps(int cnt) {
		for (int _ = 0; _ < cnt; ++_)
			MakeStep();
	}

	void MakeStep() {
		if (mode == NORMAL)	
			return;

		switch (phase) {
			case OUT:
				if (out.empty()) {
					phase = IN;
				} else {
					MOVE_ELEM(out, tmp);
					break;
				}

			case IN:
				if (in.empty()) {
					phase = TMP;
				} else {
					int x = in.pop();

					out.push(x);
					outCopy2.push(x);
					break;
				}

			case TMP:
				if (tmp.empty()) {
					phase = CLEAN;
				} else {
					int x = tmp.pop();

					out.push(x);
					outCopy2.push(x);
					break;
				}
			case CLEAN:
				if (deleted == 0) {
					phase = NORM;
				} else {
					--deleted;
					out.pop();
					outCopy2.pop();
					break;
				}

			default:
				TurnOff();
		}

		#ifdef DEBUG
		PrintInfo();
		#endif
	}

	void TurnOff() {
		mode = NORMAL;
		swap(outCopy, outCopy2);
		swap(in, inCopy);
	}

	#define PRINT_STACK(stack_name)	printf(#stack_name ": ");		\
									for (auto x : stack_name.st)	\
										printf("%d ", x); 			\
									printf("\n");

	void PrintInfo() {
		printf("\\--------------------------------\n");
		printf("mode: %d, phase: %d\n", mode, phase);
		printf("deleted: %d\n", deleted);

		PRINT_STACK(in);
		PRINT_STACK(out);
		PRINT_STACK(inCopy);
		PRINT_STACK(outCopy);
		PRINT_STACK(tmp);
		PRINT_STACK(outCopy2);

		printf("\\--------------------------------");
		cout << endl;
	}

	#undef PRINT_STACK

	Stack	in,		out, 		\
			inCopy, outCopy, 	\
			tmp,	outCopy2;

	Mode	mode	= NORMAL;
	Phase	phase	= NORM;
	int		kol		= 0;
	int		deleted	= 0;

};

#undef MOVE_ELEM

void TestQueue(int n) {
	srand(time(NULL));

	Queue q;
	queue<int> r;

	for (int i = 0; i < n; ++i) {
        int x = rand() % 1000 + 2;

		if (r.empty() || rand() % 2) {
			q.push(x);
			r.push(x);

			#ifdef DEBUG
			q.PrintInfo();
			#endif
		}
		else {
			int res1 = r.front();
			int res2 = q.pop();

			if (res1 != res2) {
				cout << "ERROR";
				return;
			}

			r.pop();

			#ifdef DEBUG
			q.PrintInfo();
			#endif
		}
    }

	printf("Everything is OK\n");
}
