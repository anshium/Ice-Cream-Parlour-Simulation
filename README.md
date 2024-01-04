# Ice-Cream-Parlour-Simulation
Sunny’s Ice Cream Parlor is renowned for its delectable ice cream creations and has garnered fame through media coverage. To meet the growing demand, Sunny has expanded the offerings, employing skilled ice cream machines. Your task is to simulate the operation of Sunny’s Ice Cream Parlor.

### DURING THE SIMULATION:
- There are N ice cream machines.
- Every ice cream machine m works from time tm_start to time tm_stop.
- There are F ice cream flavours
- Every ice cream flavour f takes unique time t_f to prepare.
- There are T toppings.
- Every topping t takes constant time t_t to put.
- A topping t is present in limited quantity, q_t
- There is a capacity of K customers in the parlour.
- A customer c arrives at time t_arr


### ASSUMPTIONS:
- Simulation begins from 0 seconds.
- If a customer arrives at time t, they place the order at time t, and the order starts getting prepared at time t+1.
- If a customer’s order can only be partially completed, they must be rejected completely, no part of their order should waste ingredients.
- If a customer leaves due to ingredient shortages, they leave the second they’re informed (at t), and their spot becomes free from the next second i.e. t+1.
- Once an entire order is completed, the customer picks it up and leaves instantaneously.
- A machine cannot start preparing an order if it will have to stop working in the middle of that order.
- If a machine starts working at time t, and a customer was already waiting since some time <t, the machine can instantly start preparing their order.

## Here’s a structured breakdown of the scenario:

### Ice Cream Machines:
- Sunny’s parlor has N ice cream machines, and they have individual work shifts.
- Ice machines unfortunately work at different times during the day.

### Ice Cream Varieties:
- Sunny offers an assortment of ice cream flavors, each with its own preparation time and ingredient requirements.
- Some ingredients, like vanilla and chocolate syrup, are available in abundance, while others, such as fresh fruits and whipped cream, are limited for the day.
- If all limited ingredients are depleted, the parlor closes for the day.

### Customers:
- The parlour can accommodate a maximum of K customers at a time. Customers can place orders for multiple ice creams with diverse flavors and toppings.
- Customers submit their orders immediately upon entering. If ingredient shortages prevent fulfillment, they receive immediate notification (after order is taken) and can choose to exit the parlour.
- If an order can be fulfilled, customers must wait for their ice creams to be prepared and brought to the pickup spot.
- Your simulation should utilize multi-threading concepts and avoid potential issues like deadlocks and busy waiting. In your report, provide implementation details and address the given follow-up questions. 

Make sure to colour code your output using the specified colours.

### INPUT FORMAT (EVERYTHING IS SPACE SEPARATED):

- - The first line contains `N K F T`
  - The next N lines contain machine timings `tm_start tm_stop`
  - The next F lines contain `f t_f`
  - The next T lines contain `t q_t` [Note: q_t will be given -1 for unlimited quantity]
- The rest of the lines will have the structure `c t_arr number of ice creams they want to order flavour of ice cream 1 topping 1 topping 2 … topping n … flavour of ice cream n topping 1 topping 2 … topping n`

### OUTPUT FORMAT:
- When a customerc enters at time t, print [white colour] Customer c enters at t second(s)
- Print their order [yellow colour]:
```
Customer c orders 2 ice creams

Ice cream 1: vanilla caramel

Ice cream 2: chocolate candy
```
- When a machine m is starts preparing the order of customer c, print [cyan colour] `Machine m starts preparing ice cream 1 of customer c at t seconds(s)`
- When a machine m is completes preparing the order of customer c, print [blue colour] `Machine m completes preparing ice cream 1 of customer c at t seconds(s)`
- When a customer’s order is complete, as they will pick it up immidiately, print [green colour] `Customer c has collected their order(s) and left at t second(s)`
- If a customer was rejected due to ingredient shortage, print [red colour] `Customer c left at t second(s) with an unfulfilled order`
- If a customer c was not serviced even when the parlour is closing (last machine has stopped working), print [red colour] `Customer c was not serviced due to unavailability of machines`
- When a machine m starts working, print [orange colour] `Machine m has started working at t second(s)`
- When a machine m stops working, print [orange colour] `Machine m has stopped working at t second(s)`

EXAMPLE:

Input:
```
2 3 2 3

0 7

4 10

vanilla 3

chocolate 4

caramel -1

brownie 4

strawberry 4

1 1 2

vanilla caramel

chocolate brownie strawberry

2 2 1

vanilla strawberry caramel
```

Output:


```
Machine 1 has started working at 0 second(s)

Customer 1 enters at 1 second(s)

Customer 1 orders 2 ice creams

Ice cream 1: vanilla caramel

Ice cream 2: chocolate brownie strawberry

Machine 1 starts preparing ice cream 1 of customer 1 at 2 second(s)

Customer 2 enters at 2 second(s)

Customer 2 orders 1 ice cream(s)

Ice cream 1: vanilla strawberry caramel

Machine 2 has started working at 4 second(s)

Machine 2 starts preparing ice cream 2 of customer 1 at 4 second(s)

Machine 1 completes preparing ice cream 1 of customer 1 at 5 seconds(s)

Machine 1 has stopped working at 7 second(s)

Machine 2 completes preparing ice cream 2 of customer 1 at 8 seconds(s)

Customer 1 has collected their order(s) and left at 8 second(s)

Machine 2 has stopped working at 10 second(s)

Customer 2 was not serviced due to unavailability of machines

Parlour Closed
```

### QUESTIONS:

1. <b>Minimizing Incomplete Orders:</b> Describe your approach to redesign the simulation to minimize incomplete orders, given that incomplete orders can impact the parlor’s reputation. Note that the parlor’s reputation 
is unaffected if orders are instantly rejected due to topping shortages.

2. <b>Ingredient Replenishment:</b> Assuming ingredients can be replenished by contacting the nearest supplier, outline how you would adjust the parlour order acceptance/rejection process based on ingredient 
availability.

3. <b>Unserviced Orders:</b> Suggest ways by which we can avoid/minimise the number of unserviced orders or customers having to wait until the parlor closes.
