"""Defines standard progress bars to avoid code duplication. Wrapppers around rich progress bars. See DefaultProgressBar for an example of usage."""

from rich.progress import (
    BarColumn,
    MofNCompleteColumn,
    Progress,
    TextColumn,
    TimeElapsedColumn,
    TimeRemainingColumn
)
class DefaultProgressBar(Progress):
    """A standard progress bar for the command line. It is a wrapper around rich.progress.Progress.
    
    Usage:
    ```python
    with DefaultProgressBar() as progress:
        progress.set_total_steps(total_size)
        for ...:
            ...
            progress.next(description=f"...")
    ```
    """
    def __init__(self):
        super(DefaultProgressBar, self).__init__(
            TextColumn("[progress.description]{task.description}"),
            BarColumn(),"[progress.percentage]{task.percentage:>3.1f}%",
            TimeElapsedColumn(),
            TimeRemainingColumn(),
            MofNCompleteColumn()
        )
    def __enter__(self) -> "DefaultProgressBar":
        super(DefaultProgressBar, self).__enter__()
        return self
    def set_total_steps(self,n: int):
        """Set the total number of times the next method will be called. Maximum progress"""
        self.task = self.add_task("[green]{task.description}", total=n)
    def next(self,description: str):
        """Indicates that one step has been completed. The description is updated with the provided string"""
        self.update(self.task, description=f"[green]{description} ...", advance=1)
    