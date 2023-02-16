def mxy_function(name, age):
    print('welcome home '+name+ ' and you are '+str(age)+' years old')
    
name = input('Enter your name here: ')
age = int(input('Enter your age here: '))
mxy_function(name, age)

if age%5 == 0:
    print('Did you know that your age is divisible by 5')
else:
    print('Did you know that your age is not divisible by 5')

def add_numbers(num1, num2):
	print('the sum is:')
	return num1 + num2
num1 = int(input('Enter the first number: '))
num2 = int(input('Enter the second number: '))
print(add_numbers(num1, num2))
